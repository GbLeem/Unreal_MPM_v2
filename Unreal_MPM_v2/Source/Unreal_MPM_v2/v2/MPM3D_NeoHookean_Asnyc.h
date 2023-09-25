#pragma once

#include "Async/Async.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_NeoHookean_Asnyc.generated.h"

using namespace Chaos;

UCLASS()
class UNREAL_MPM_V2_API AMPM3D_NeoHookean_Asnyc : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPM3D_NeoHookean_Asnyc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ClearGrid();
	void P2G();
	void UpdateGrid();
	void G2P();
	
	void Simulate();
	void UpdateParticles();

public:
	struct Cell
	{
		FVector3f v;
		float mass;
	};

	struct Particle
	{
		FVector3f x;
		FVector3f v;
		PMatrix<float, 3, 3> C; //affine momentum matrix
		float mass;
		float volume_0;
	};

public:
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	int NumParticles = 0;

	const int grid_res = 64;
	const int32 NumCells = grid_res * grid_res * grid_res;

	const float dt = 0.1f;
	const float iterations = (int)(1.f / dt);
	const float gravity = -0.3f;

	const float elastic_lambda = 15.f;
	const float elastic_mu = 400.f;

	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<PMatrix<float, 3, 3>> Fs;
	TArray<FVector3f> weights;
	TArray<FTransform> Transforms;

	FRunnableThread* MyThread;
};

class SimulateTask :public FRunnable
{
public:
	SimulateTask(AMPM3D_NeoHookean_Asnyc* mpm)
		:Owner(mpm) 
	{
	}

	~SimulateTask()
	{
		if (Owner)
		{
			//Owner->Kill();
			delete Owner;
		}
	}
	bool Init() override
	{
		UE_LOG(LogTemp, Warning, TEXT("My custom thread has been initialized"));
		return true;
	}

	uint32 Run() override
	{
		while (bRunThread)
		{
			UE_LOG(LogTemp, Warning, TEXT("My custom thread is running!"));

			//Owner->ClearGrid();
			//Owner->P2G();
			//Owner->UpdateGrid(); //[0925 : INDEX ERROR]
			//Owner->G2P();
			//Owner->UpdateParticles();
			Owner->Simulate();
		}
		return 0;
	}

	void Stop() override
	{
		bRunThread = false;
	}
private:
	AMPM3D_NeoHookean_Asnyc* Owner;
	bool bRunThread;
};