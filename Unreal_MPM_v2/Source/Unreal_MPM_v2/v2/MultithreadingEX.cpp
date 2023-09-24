// Fill out your copyright notice in the Description page of Project Settings.


#include "MultithreadingEX.h"

// Sets default values
AMultithreadingEX::AMultithreadingEX()
	:ContainerSize(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("Constructor"));

}

// Called when the game starts or when spawned
void AMultithreadingEX::BeginPlay()
{
	Super::BeginPlay();
	
	ContainerSize = 10000000;
	
	for (int i = 0; i < ContainerSize; ++i)
	{
		TestContainer.Add(1);
	}

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : %d"), ContainerSize);	
}

// Called every frame
void AMultithreadingEX::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FRunnableThread* MyThread = FRunnableThread::Create(new Simulate(this), TEXT("MyThread"), 0, TPri_Normal);
	Function1();
	Function2();
	Function3();
}

void AMultithreadingEX::Function1()
{
	for (int i = 0; i < 500; ++i)
	{
		TestContainer[i] = i * 2; //[INDEX ERROR]
	}
	UE_LOG(LogTemp, Warning, TEXT("Function1"));

}

void AMultithreadingEX::Function2()
{
	for (int i = 0; i < ContainerSize; ++i)
	{
		if (TestContainer[i] % 2 == 0)
			TestContainer[i] = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("Function2"));

}

void AMultithreadingEX::Function3()
{
	for (int i = 0; i < 100; ++i)
	{
		TestContainer[i] = 100;
	}
	UE_LOG(LogTemp, Warning, TEXT("Function3"));

}

