#pragma once

#include "Components/InstancedStaticMeshComponent.h"
#include "Chaos/Matrix.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPM3D_NeoHookean_v2.generated.h"

//using namespace Chaos;

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
	void NewP2G();
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
		Chaos::PMatrix<float, 3, 3> C; //affine momentum matrix
		float mass;
		float volume_0;
	};

//public:
//	//v2
//	class PClearGrid : public FNonAbandonableTask
//	{
//		TArray<Cell*> m_pNewGrid;
//
//		PClearGrid(TArray<Cell*> grid)
//			:m_pNewGrid(grid)
//		{
//		}
//		void DoWork()
//		{
//			for (int32 i = 0; i < m_pNewGrid.Num(); ++i) {
//				Cell* Cell = m_pNewGrid[i];
//
//				// reset grid scratch-pad entirely
//				Cell->mass = 0;
//				Cell->v = { 0.f,0.f,0.f };
//			}
//		}
//		FORCEINLINE TStatId GetStatId() const {
//			RETURN_QUICK_DECLARE_CYCLE_STAT(PClearGrid, STATGROUP_ThreadPoolAsyncTasks);
//		}
//	};
//
//	class FClearGridTask : public FTaskThreadInterface {
//	public:
//		FClearGridTask(TArray<Cell*> grid) : m_pNewGrid(grid) {}
//
//		TArray<Cell*>& m_pNewGrid;
//
//		void DoWork() {
//			PClearGrid ClearGridJob(m_pNewGrid);
//			TGraphTask<PClearGrid>::CreateTask().ConstructAndDispatchWhenReady(ClearGridJob, ENamedThreads::AnyThread);
//		}
//
//
//		virtual FName GetName() const {
//			static FName TaskName(TEXT("FClearGridTask"));
//			return TaskName;
//		}
//	};

public:
	UPROPERTY(BluePrintReadWrite, VisibleAnywhere)
		UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	int NumParticles = 0;

	const int grid_res = 64;
	const int NumCells = grid_res * grid_res * grid_res;

	const float dt = 0.1f;
	const float iterations = (int)(1.f / dt);
	const float gravity = -0.3f;

	const float elastic_lambda = 15.f;
	const float elastic_mu = 500.f;

	bool P2GFirst = true;

	TArray<Particle*> m_pParticles;
	TArray<Cell*> m_pGrid;
	TArray<Chaos::PMatrix<float, 3, 3>> Fs;
	TArray<FVector3f> weights;
	TArray<FTransform> Transforms;
};
