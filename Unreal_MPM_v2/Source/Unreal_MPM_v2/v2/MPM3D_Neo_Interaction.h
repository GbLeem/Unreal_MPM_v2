// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_Neo_Interaction.generated.h"

using namespace Chaos;

UCLASS()
class UNREAL_MPM_V2_API AMPM3D_Neo_Interaction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPM3D_Neo_Interaction();

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

	int NumParticles1 = 0;
	int NumParticles2 = 0;
	int TotalParticles = 0;

	const int grid_res = 64;
	const int NumCells = grid_res * grid_res * grid_res;

	const float dt = 0.1f;
	const float iterations = (int)(1.f / dt);
	const float gravity = -0.5f;

	const float elastic_lambda = 10.f;
	const float elastic_mu = 500.f;

	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<PMatrix<float, 3, 3>> Fs;
	TArray<FVec3f> weights;
	TArray<FTransform> Transforms;

	mutable FCriticalSection myMutex;
};
