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
//	Simulate* Worker = new Simulate();

};

class Simulate :public FRunnable
{
public:
	Simulate()
	{
		//FRunnableThread* MyThread = FRunnableThread::Create(new Simulate(), TEXT("MyThread"), 0, TPri_Normal);
		Thread = FRunnableThread::Create(this, TEXT("Give your thread a good name"));
	}
	virtual ~Simulate() override
	{
		if (Thread)
		{
			// Kill() is a blocking call, it waits for the thread to finish.
			// Hopefully that doesn't take too long
			Thread->Kill();
			delete Thread;
		}
	}
	bool Init() override
	{
		UE_LOG(LogTemp, Warning, TEXT("My custom thread has been initialized"))

		// Return false if you want to abort the thread
		return true;
	}

	uint32 Run() override
	{
		/*Owner->Function1();
		Owner->Function2();
		Owner->Function3();
		return 0;*/
		while (bRunThread)
		{
			if (bInputReady)
			{
				// Do your intensive tasks here. You can safely modify all input variables.
				// For the example, I'm just going to convert the input int to a float
				ExampleFloatOutput = ExampleIntInput;
				FPlatformProcess::Sleep(1.0f); // Simulate a heavy workload

				// Do this once you've finished using the input/output variables
				// From this point onwards, do not touch them!
				bInputReady = false;

				// I hear that it's good to let the thread sleep a bit, so the OS can utilise it better or something.
				FPlatformProcess::Sleep(0.01f);
			}

		}
		return 0;
	}

	void Stop() override 
	{
		// 중지 작업 (옵션)
		bRunThread = false;
	}

	bool bInputReady = false;
	int ExampleIntInput = 0;
	float ExampleFloatOutput = 0.0f;
private:
	FRunnableThread* Thread;
	bool bRunThread;
	//AMultithreadingEX* Owner;
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