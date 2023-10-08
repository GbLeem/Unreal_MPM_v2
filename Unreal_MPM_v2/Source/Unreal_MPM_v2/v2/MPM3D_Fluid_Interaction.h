#pragma once

#include "../v1/MPM3D_fluid.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_Fluid_Interaction.generated.h"


UCLASS()
class UNREAL_MPM_V2_API AMPM3D_Fluid_Interaction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPM3D_Fluid_Interaction();

protected:
	// Called when the game starts or when spawned
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

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Ball;

	FTransform BallPos;

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
