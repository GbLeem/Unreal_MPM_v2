// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsyncTaskActor.generated.h"

UCLASS()
class UNREAL_MPM_V2_API AAsyncTaskActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsyncTaskActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void StartAsyncTask();
	void StartAsyncTask_UsingAsyncTask();
};

class FMyAsyncTask : public FNonAbandonableTask
{
public:
	FMyAsyncTask(int32 InLoopCount) : LoopCount(InLoopCount) {}
	FORCEINLINE TStatId GetStatId() const 
	{ RETURN_QUICK_DECLARE_CYCLE_STAT(FMyAsyncTask, STATGROUP_ThreadPoolAsyncTasks); }
	void DoWork();

private:
	int32 LoopCount;
};