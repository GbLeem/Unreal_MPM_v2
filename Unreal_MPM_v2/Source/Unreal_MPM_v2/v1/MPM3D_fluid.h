// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_fluid.generated.h"

using namespace Chaos;

UCLASS()
class UNREAL_MPM_V2_API AMPM3D_fluid : public AActor
{
	GENERATED_BODY()
	
public:	
	AMPM3D_fluid();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void ClearGrid();
	void P2G_1();
	void P2G_2();
	void UpdateGrid();
	void G2P();

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
	};

public:
	UPROPERTY(VisibleAnywhere)
		UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	int NumParticles;

	const int grid_res = 32;
	const int NumCells = grid_res * grid_res * grid_res;

	const float dt = 0.2f;
	const float gravity = -1.0f;

	const float rest_density = 4.0f;
	const float dynamic_viscosity = 0.2f;

	const float eos_stiffness = 20.f;
	const float eos_power = 4.f;

	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<FVector3f> weights;
};
