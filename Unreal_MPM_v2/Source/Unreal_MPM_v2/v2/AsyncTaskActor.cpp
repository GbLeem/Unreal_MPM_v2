#include "AsyncTaskActor.h"

AAsyncTaskActor::AAsyncTaskActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAsyncTaskActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartAsyncTask();
	StartAsyncTask_UsingAsyncTask();
}

void AAsyncTaskActor::StartAsyncTask()
{
	(new FAutoDeleteAsyncTask<FMyAsyncTask>(10))->StartBackgroundTask();
}

void AAsyncTaskActor::StartAsyncTask_UsingAsyncTask()
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, []()
		{
			auto MyTask = new FAsyncTask<FMyAsyncTask>(10);
	MyTask->StartBackgroundTask();
	MyTask->EnsureCompletion();
	delete MyTask;
	UE_LOG(LogTemp, Log, TEXT("[MyLog] Stop : AsyncTask"));
		});
}

void FMyAsyncTask::DoWork()
{
	while (LoopCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("[MyLog] Run - NonAbandaonable: %d"), LoopCount--);
		FPlatformProcess::Sleep(0.5f);
	}
	UE_LOG(LogTemp, Log, TEXT("[MyLog] Stop - NonAbanbleTask: %d"), LoopCount--);
}
