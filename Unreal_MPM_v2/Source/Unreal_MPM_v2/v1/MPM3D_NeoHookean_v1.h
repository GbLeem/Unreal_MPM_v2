#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_NeoHookean_v1.generated.h"

using namespace Chaos;

UCLASS()
class UNREAL_MPM_V2_API AMPM3D_NeoHookean_v1 : public AActor
{
	GENERATED_BODY()
	
public:	
	AMPM3D_NeoHookean_v1();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void ClearGrid();
	void P2G();
	void UpdateGrid();
	void G2P();

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
		PMatrix<float, 3, 3> C; //affine momentum matrix
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
	const float iterations = (int)(1.f / dt);
	const float gravity = -0.5f;

	const float elastic_lambda = 15.f;
	const float elastic_mu = 500.f;

	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<PMatrix<float, 3, 3>> Fs;
	TArray<FVec3f> weights;
	TArray<FTransform> Transforms;

	FCriticalSection Mutex;
};
