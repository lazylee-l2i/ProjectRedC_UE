// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MoveToWaypoint.h"
#include "AI/AIController/RedCAIController.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Spawn/RedCAISpawnActor.h"
#include "AIController.h"



UBTTask_MoveToWaypoint::UBTTask_MoveToWaypoint()
{
    NodeName = TEXT("Move To Waypoint");
    
    bNotifyTick = true;
    bNotifyTaskFinished = true;
    bCreateNodeInstance = true;
    
    OwnerCompPtr = nullptr;
    AIController = nullptr;
    AICharacter = nullptr;
    
    MoveRequestID = FAIRequestID::InvalidRequest;
}

void UBTTask_MoveToWaypoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!AICharacterWeak.IsValid() || !AIControllerWeak.IsValid())
    {
        return;
    }
    
    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        if (BB->GetValueAsBool("bIsAggroed"))
        {
            CleanupDelegates();
            FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
        }
    }
}

EBTNodeResult::Type UBTTask_MoveToWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    
    OwnerCompPtr = &OwnerComp;
    OwnerCompWeak = &OwnerComp;

    if (MoveRequestID.IsValid())
    {
        return EBTNodeResult::InProgress;
    }
    
    AIController = Cast<ARedCAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return EBTNodeResult::Failed;
    
    AICharacter =  Cast<ARedCAICharacter>(AIController->GetPawn());
    if (!AICharacter || !AICharacter->IsAlive())
    {
        return EBTNodeResult::Failed;
    }
    
    AIControllerWeak      = AIController;
    AICharacterWeak       = AICharacter;
    CapturedGenerationId = AICharacter->GenerationId;

    // 스포너 캐시는 "Owner 우선 → 캐릭터 약포인터(OwnerSpawnerWeak) 보조" 순으로 '한 번'만 결정
    {
        ARedCAISpawnActor* SpawnerByOwner = Cast<ARedCAISpawnActor>(AICharacter->GetOwner());
        if (SpawnerByOwner)
        {
            SpawnerWeak = SpawnerByOwner;
        }
        else
        {
            // 반드시 새 필드(약포인터) 사용!
            SpawnerWeak = AICharacter->OwnerSpawnerWeak;   
        }

        if (!SpawnerWeak.IsValid())
        {
            // 스포너가 없으면 이후 콜백에서 큰 사고 → 지금 실패 처리
            return EBTNodeResult::Failed;
        }
    }
    //인덱스 보정 NumWPs 웨이포인트 번호
    const int32 NumWPs = AICharacter->Waypoints.Num();
    if (NumWPs <= 0) return EBTNodeResult::Failed;

    
    // 현재 인덱스 이후의 웨이포인트 중에서 가장 가까운 것 탐색
    float ClosestDistSq = FLT_MAX;
    int32 ClosestIndex = AICharacter->CurrentWaypointIndex; // 기본값: 현재 인덱스 유지

    const FVector CurrentLocation = AICharacter->GetActorLocation();

    for (int32 i = AICharacter->CurrentWaypointIndex; i < NumWPs; ++i)
    {
        if (AActor* WP = AICharacter->Waypoints[i])
        {
            const float DistSq = FVector::DistSquared(CurrentLocation, WP->GetActorLocation());
            if (DistSq < ClosestDistSq)
            {
                ClosestDistSq = DistSq;
                ClosestIndex = i;
            }
        }
    }

    // 가까운 다음 구간 웨이포인트로 인덱스 갱신
    AICharacter->CurrentWaypointIndex = ClosestIndex;

    
    // 현재 타겟 웨이포인트
    AActor* Target = AICharacter->Waypoints[AICharacter->CurrentWaypointIndex];
    if (!IsValid(Target))
    {
        return EBTNodeResult::Failed;
    }

    const bool bIsLast = (AICharacter->CurrentWaypointIndex == NumWPs - 1);
    
    // FAIMoveRequest 생성
    FAIMoveRequest MoveReq;

    if (bIsLast)
    {
        MoveReq.SetGoalActor(Target);
        MoveReq.SetAcceptanceRadius(100.f);
    }
    else
    {
        MoveReq.SetGoalLocation(Target->GetActorLocation());
        MoveReq.SetAcceptanceRadius(75.f);     // 반경 약간 줄임
    }
    
    // 경로 디버깅용 OutPath
    FNavPathSharedPtr NavPath;
    FPathFollowingRequestResult Result = AIController->MoveTo(MoveReq, &NavPath);
    
    if (Result.Code != EPathFollowingRequestResult::RequestSuccessful)
    {
        return EBTNodeResult::Failed;
    }
    
    MoveRequestID = Result.MoveId; // 중복 호출 제거
   
    // PathFollowingComponent에 델리게이트 연결
   UPathFollowingComponent* PFC = AIController->GetPathFollowingComponent();
        if (PFC)
        {
            if (MoveFinishedHandle.IsValid())
            {
                PFC->OnRequestFinished.Remove(MoveFinishedHandle);  // RemoveAll(this) 대신 정확히 핸들 제거
                MoveFinishedHandle.Reset();
            }
            MoveFinishedHandle = PFC->OnRequestFinished.AddUObject(this, &UBTTask_MoveToWaypoint::OnMoveCompleted);
        }
    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_MoveToWaypoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (bFinishing) return EBTNodeResult::Aborted;
    
    // 이동 중단
    if (AIController && IsValid(AIController))
    {
        AIController->StopMovement();
    }
    CleanupDelegates();
    
    return EBTNodeResult::Aborted;
}

void UBTTask_MoveToWaypoint::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    
    CleanupDelegates();

    //weakPtr 초기화
    OwnerCompWeak.Reset();
    AIControllerWeak.Reset();
    AICharacterWeak.Reset();
    SpawnerWeak.Reset();
    
    // 참조 초기화
    OwnerCompPtr = nullptr;
    AIController = nullptr;
    AICharacter = nullptr;
    
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_MoveToWaypoint::OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result)
{
    //재진입 차단
    if (bFinishing) return;
    bFinishing = true;
    
    // 포인터 유효성은 "캐시" 기준으로 검사 (ExecuteTask에서 채워두기)
    UBehaviorTreeComponent* OwnerComp = OwnerCompWeak.IsValid() ? OwnerCompWeak.Get() : nullptr;
    ARedCAIController*      Ctl       = AIControllerWeak.IsValid() ? AIControllerWeak.Get() : nullptr;
    ARedCAICharacter*       Chr       = AICharacterWeak.IsValid() ? AICharacterWeak.Get() : nullptr;

    if (!OwnerComp || !Ctl || !Chr ||
        !OwnerComp->IsActive() ||
        Ctl->GetPawn() != Chr||
        !Chr->bIsActiveInWorld ||
        Ctl->GetPawn() != Chr)
    {
        CleanupDelegates();
        return;
    }
    
   
    // 오래된 요청이면 무시
    if (!MoveRequestID.IsValid() || RequestID != MoveRequestID)
    {
        CleanupDelegates();
        return;
    }
    
    MoveRequestID = FAIRequestID::InvalidRequest;
    
    if (!AICharacter || AICharacter->GenerationId != CapturedGenerationId)
    {
        // 이전 세대 콜백이면 바로 탈출
        CleanupDelegates();
        return;
    }
    
   
 
    if (Result.Code == EPathFollowingResult::Success)
    {
        
            const int32 NumWPs    = Chr->Waypoints.Num();
            const int32 NextIndex = Chr->CurrentWaypointIndex + 1;

            if (NextIndex >= NumWPs)
            {
                // 스포너/캐릭터는 "캐시된 WeakPtr"만 사용
                TWeakObjectPtr<ARedCAICharacter>  WeakChar    = AICharacterWeak;
                TWeakObjectPtr<ARedCAISpawnActor> WeakSpawner = SpawnerWeak;

                // WeakPtr 안전 접근: 먼저 raw로 숏서킷 → 필요시 IsValid
                ARedCAISpawnActor* RawSpawner = WeakSpawner.Get();
                if (WeakChar.IsValid() && RawSpawner != nullptr)
                {
                    if (UWorld* World = WeakChar->GetWorld())
                    {
                        World->GetTimerManager().SetTimerForNextTick(
                            FTimerDelegate::CreateLambda([WeakChar, WeakSpawner]()
                            {
                                if (WeakChar.IsValid() && WeakSpawner.IsValid())
                                {
                                  //  WeakSpawner->ReturnAIToPool(WeakChar.Get());
                                }
                            })
                        );
                    }
                }

                //  예약을 끝낸 뒤 마지막에 태스크 종료
                FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
                return;
            }

            // 마지막이 아니면 인덱스 반영 후 종료
            Chr->CurrentWaypointIndex = NextIndex;
            FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
        // 실패
        FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_MoveToWaypoint::CleanupDelegates()
{
    if (ARedCAIController* Ctl = AIControllerWeak.Get())
    {
        if (UPathFollowingComponent* PFC = Ctl->GetPathFollowingComponent())
            {
                if (MoveFinishedHandle.IsValid())
                {
                    PFC->OnRequestFinished.Remove(MoveFinishedHandle);
                    MoveFinishedHandle.Reset();
                }
            }
        }
        MoveRequestID = FAIRequestID::InvalidRequest;
        bFinishing = false;
}