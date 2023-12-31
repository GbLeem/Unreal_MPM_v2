#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_NeoHookean_v2.generated.h"

using namespace Chaos;

UCLASS()
class UNREAL_MPM_V2_API AMPM3D_NeoHookean_v2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPM3D_NeoHookean_v2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnBox();
	void Scatter();

	//v1
	void ClearGrid();
	void P2G();
	void UpdateGrid();
	void G2P();
	
	//void Simulate();
	void UpdateParticles();

public:
	struct Cell
	{
		FVec3f v;
		float mass;
	};

	struct Particle
	{
		FVec3f x;
		FVec3f v;
		FMatrix33 C; //affine momentum matrix
		float mass;
		float volume_0;
	};

public:
	UPROPERTY(BluePrintReadWrite, VisibleAnywhere)
		UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	int NumParticles = 0;

	const int grid_res = 64;
	const int NumCells = grid_res * grid_res * grid_res;

	const float dt = 0.1f;
	const float gravity = -0.5f;

	const float elastic_lambda = 15.f;
	const float elastic_mu = 500.f;


	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<FMatrix33> Fs;
	//TArray<FVector3f> weights;
	TArray<FVec3f> weights;
	
	TArray<FTransform> Transforms;
	TArray<FVec3f> TempPositions;

	/*FCriticalSection Mutex;
	FCriticalSection MutexForParticle;*/
};
