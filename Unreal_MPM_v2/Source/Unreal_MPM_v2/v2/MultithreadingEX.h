// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Async/Async.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultithreadingEX.generated.h"
UCLASS()
class UNREAL_MPM_V2_API AMultithreadingEX : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultithreadingEX();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Function1();
	void Function2();
	void Function3();

	TArray<int> TestContainer;
	int ContainerSize;
};

class Simulate :public FRunnable
{
public:
	Simulate(AMultithreadingEX* ex)
		:Owner(ex){}

	bool Init() override
	{
		return true;
	}

	uint32 Run() override
	{
		Owner->Function1();
		Owner->Function2();
		Owner->Function3();
		return 0;
	}

	void Stop() override 
	{
		// 중지 작업 (옵션)
	}

	void Exit() override 
	{
		// 종료 작업 (옵션)
	}
private:
	AMultithreadingEX* Owner;
};



//class MyMultiThreading : public FNonAbandonableTask
//{
//
//private:
//	MyMultiThreading();
//
//	FORCEINLINE TStatId GetStatId() const
//	{
//		RETURN_QUICK_DECLARE_CYCLE_STAT(MyMultiThreading, STATGROUP_ThreadPoolAsyncTasks);
//	}
//	void DoWork();
//private:
//
////public:
////	FMyAsyncTask(int32 InLoopCount) : LoopCount(InLoopCount) {}
////	FORCEINLINE TStatId GetStatId() const
////	{
////		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
////	}
////	void DoWork();
////
////private:
////	int32 LoopCount;
//
//};