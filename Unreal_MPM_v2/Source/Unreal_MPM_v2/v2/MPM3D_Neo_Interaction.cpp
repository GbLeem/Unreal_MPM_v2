
#include "MPM3D_Neo_Interaction.h"

AMPM3D_Neo_Interaction::AMPM3D_Neo_Interaction()
{
	PrimaryActorTick.bCanEverTick = true;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));

	SetRootComponent(InstancedStaticMeshComponent);

	InstancedStaticMeshComponent->SetMobility(EComponentMobility::Static);
	InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	InstancedStaticMeshComponent->SetGenerateOverlapEvents(false);

}

void AMPM3D_Neo_Interaction::BeginPlay()
{
	Super::BeginPlay();

	TArray<FVec3f> TempPositions;
	TArray<FVec3f> TempPositions2;

	const float spacing = 0.5f;
	const int box = 4;
	const float s = 32.f;

	for (float i = -box; i < box; i += spacing) 
	{
		for (float j = -box; j < box; j += spacing)
		{
			for (float k = -box; k < box; k += spacing)
			{
				TempPositions.Add(FVec3f{ s + i, s + j, s + k });
			}
		}
	}

	for (float i = -box; i < box; i += spacing)
	{
		for (float j = -box - 2; j < box - 2; j += spacing)
		{
			for (float k = -box - 10; k < box - 10; k += spacing)
			{
				TempPositions2.Add(FVec3f{ s + i, s + j, s + k });
			}
		}
	}

	NumParticles1 = TempPositions.Num();
	NumParticles2 = TempPositions2.Num();
	TotalParticles = NumParticles1 + NumParticles2;

	//UE_LOG(LogTemp, Warning, TEXT("%d"), NumParticles);
	for (int i = 0; i < NumParticles1; ++i)
	{
		Particle* p = new Particle();
		p->x = TempPositions[i];
		p->v = { 0.f,0.f,0.f };
		p->C;
		p->mass = 1.f;

		m_pParticles.Add(p);

		Fs.Add(FMatrix33{ 1.f, 1.f, 1.f });
	}
	for (int i = 0; i < NumParticles2; ++i)
	{
		Particle* p = new Particle();
		p->x = TempPositions2[i];
		p->v = { 0.f,0.f,0.f };
		p->C;
		p->mass = 1.f;

		m_pParticles.Add(p);

		Fs.Add(FMatrix33{ 1.f, 1.f, 1.f });
	}

	m_pGrid.Empty(NumCells);

	for (int i = 0; i < NumCells; ++i)
	{
		Cell* cell = new Cell();
		cell->v = { 0.f, 0.f, 0.f };
		m_pGrid.Add(cell);
	}

	P2G();

	for (int i = 0; i < TotalParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		FVec3f cell_idx{ floorf(p->x.X), floorf(p->x.Y) ,floorf(p->x.Z) };
		FVec3f cell_diff{ (p->x.X - cell_idx.X - 0.5f), (p->x.Y - cell_idx.Y - 0.5f), (p->x.Z - cell_idx.Z - 0.5f) };

		weights.Empty(3);
		weights.Add({ 0.5f * (float)pow(0.5f - cell_diff.X, 2), 0.5f * (float)pow(0.5f - cell_diff.Y, 2),0.5f * (float)pow(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - (float)pow(cell_diff.X, 2), 0.75f - (float)pow(cell_diff.Y, 2), 0.75f - (float)pow(cell_diff.Z, 2) });
		weights.Add({ 0.5f * (float)pow(0.5f + cell_diff.X, 2), 0.5f * (float)pow(0.5f + cell_diff.Y, 2), 0.5f * (float)pow(0.5f + cell_diff.Z, 2) });

		float density = 0.f;

		for (int gx = 0; gx < 3; ++gx)
		{
			for (int gy = 0; gy < 3; ++gy)
			{
				for (int gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					int cell_index = ((int)(cell_idx.X) + gx - 1) * grid_res * grid_res + ((int)cell_idx.Y + gy - 1) * grid_res + ((int)cell_idx.Z + gz - 1);
					density += m_pGrid[cell_index]->mass * weight;
				}
			}
		}
		float volume = p->mass / density;
		p->volume_0 = volume;

		m_pParticles[i] = p;
	}

	if (InstancedStaticMeshComponent->GetInstanceCount() == 0)
	{
		Transforms.Empty(TotalParticles);

		ParallelFor(TotalParticles, [&](int32 i)
			{
				Transforms.Add(FTransform(FVec3{ m_pParticles[i]->x.X * 100., m_pParticles[i]->x.Y * 100., m_pParticles[i]->x.Z * 100. }));
			},true);

		//for (int i = 0; i < TotalParticles; ++i)
		//{
		//	//FTransform tempValue = ;
		//	Transforms.Add(FTransform(FVec3{ m_pParticles[i]->x.X * 100., m_pParticles[i]->x.Y * 100., m_pParticles[i]->x.Z * 100. }));
		//}
		InstancedStaticMeshComponent->AddInstances(Transforms, false);
	}
}

// Called every frame
void AMPM3D_Neo_Interaction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ClearGrid();
	P2G();
	UpdateGrid();
	G2P();
	UpdateParticles();
}

void AMPM3D_Neo_Interaction::ClearGrid()
{
	ParallelFor(NumCells, [&](int32 i)
		{
			Cell* cell = m_pGrid[i];

			cell->mass = 0;
			cell->v = { 0.f,0.f,0.f };

			m_pGrid[i] = cell;
		});

	/*for (int i = 0; i < NumCells; ++i)
	{
		Cell* cell = m_pGrid[i];

		cell->mass = 0;
		cell->v = { 0.f,0.f,0.f };

		m_pGrid[i] = cell;
	}*/
}

void AMPM3D_Neo_Interaction::P2G()
{
	for (int i = 0; i < TotalParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		PMatrix<float, 3, 3> stress{ 0.f,0.f,0.f };

		PMatrix<float, 3, 3> F = Fs[i];

		float J = F.RotDeterminant();

		float volume = p->volume_0 * J;

		PMatrix<float, 3, 3> F_inv_T = F.GetTransposed().InverseFast();

		PMatrix<float, 3, 3> P_term_0 = elastic_mu * (F - F_inv_T);
		PMatrix<float, 3, 3> P_term_1 = elastic_lambda * log(J) * F_inv_T;
		PMatrix<float, 3, 3> P = P_term_0 + P_term_1;

		stress = (1.f / J) * (P * F.GetTransposed());

		PMatrix<float, 3, 3> eq_16_term_0 = -volume * 2 * stress * dt; // [digit 2 is hyper parameter]

		TVec3<int> cell_idx{ (int)p->x.X, (int)p->x.Y, (int)p->x.Z };
		FVec3f cell_diff{ p->x.X - cell_idx.X - 0.5f, p->x.Y - cell_idx.Y - 0.5f, p->x.Z - cell_idx.Z - 0.5f };

		weights.Empty(3);
		weights.Add({ 0.5f * powf(0.5f - cell_diff.X, 2), 0.5f * powf(0.5f - cell_diff.Y, 2),0.5f * powf(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - powf(cell_diff.X, 2), 0.75f - powf(cell_diff.Y, 2), 0.75f - powf(cell_diff.Z, 2) });
		weights.Add({ 0.5f * powf(0.5f + cell_diff.X, 2), 0.5f * powf(0.5f + cell_diff.Y, 2), 0.5f * powf(0.5f + cell_diff.Z, 2) });

		for (int gx = 0; gx < 3; ++gx)
		{
			for (int gy = 0; gy < 3; ++gy)
			{
				for (int gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					TVec3<int> cell_x{ cell_idx.X + gx - 1, cell_idx.Y + gy - 1, cell_idx.Z + gz - 1 };
					FVec3f cell_dist{ cell_x.X - p->x.X + 0.5f, cell_x.Y - p->x.Y + 0.5f, cell_x.Z - p->x.Z + 0.5f };
					FVec3f Q = p->C * cell_dist;

					int cell_index = (int)cell_x.X * grid_res * grid_res + (int)cell_x.Y * grid_res + (int)cell_x.Z;

					Cell* cell = m_pGrid[cell_index];
					float weighted_mass = weight * p->mass;
					cell->mass += weighted_mass;

					cell->v += weighted_mass * (p->v + Q);

					FVec3f momentum = (eq_16_term_0 * weight) * cell_dist;
					cell->v += momentum;

					m_pGrid[cell_index] = cell;
				}
			}
		}
	}
}

void AMPM3D_Neo_Interaction::UpdateGrid()
{
	ParallelFor(NumCells, [&](int32 i)
		{
			Cell* cell = m_pGrid[i];

			if (cell->mass > 0)
			{
				cell->v /= cell->mass;
				cell->v += dt * FVector3f(0, 0, gravity);

				int x = i / (grid_res * grid_res);
				int y = (i % (grid_res * grid_res)) / grid_res;
				int z = i % grid_res;

				if (x < 2 || x > grid_res - 3)
				{
					cell->v.X = 0;
				}
				if (y < 2 || y > grid_res - 3)
				{
					cell->v.Y = 0;
				}
				if (z < 2 || z > grid_res - 3)
				{
					cell->v.Z = 0;
				}
				m_pGrid[i] = cell;
			}
		});
	/*for (int i = 0; i < NumCells; ++i)
	{
		Cell* cell = m_pGrid[i];

		if (cell->mass > 0)
		{
			cell->v /= cell->mass;
			cell->v += dt * FVec3f(0, 0, gravity);

			int x = i / (grid_res * grid_res);
			int y = (i % (grid_res * grid_res)) / grid_res;
			int z = i % grid_res;

			if (x < 2 || x > grid_res - 3)
			{
				cell->v.X = 0;
			}
			if (y < 2 || y > grid_res - 3)
			{
				cell->v.Y = 0;
			}
			if (z < 2 || z > grid_res - 3)
			{
				cell->v.Z = 0;
			}
			m_pGrid[i] = cell;
		}
	}*/
}

void AMPM3D_Neo_Interaction::G2P()
{
	for (int i = 0; i < TotalParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		p->v = { 0.f, 0.f, 0.f };

		FIntVector cell_idx = FIntVector(p->x.X, p->x.Y, p->x.Z);
		FVec3f cell_diff = { p->x.X - cell_idx.X - 0.5f, p->x.Y - cell_idx.Y - 0.5f, p->x.Z - cell_idx.Z - 0.5f };

		weights.Empty(3);
		weights.Add({ 0.5f * powf(0.5f - cell_diff.X, 2), 0.5f * powf(0.5f - cell_diff.Y, 2), 0.5f * powf(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - powf(cell_diff.X, 2), 0.75f - powf(cell_diff.Y, 2), 0.75f - powf(cell_diff.Z, 2) });
		weights.Add({ 0.5f * powf(0.5f + cell_diff.X, 2), 0.5f * powf(0.5f + cell_diff.Y, 2), 0.5f * powf(0.5f + cell_diff.Z, 2) });

		PMatrix<float, 3, 3> B{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

		for (int gx = 0; gx < 3; ++gx)
		{
			for (int gy = 0; gy < 3; ++gy)
			{
				for (int gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					TVec3<int> cell_x{ cell_idx.X + gx - 1, cell_idx.Y + gy - 1, cell_idx.Z + gz - 1 };
					int cell_index = cell_x.X * grid_res * grid_res + cell_x.Y * grid_res + cell_x.Z;

					FVec3f dist{ (cell_x.X - p->x.X) + 0.5f, (cell_x.Y - p->x.Y) + 0.5f, (cell_x.Z - p->x.Z) + 0.5f };
					FVec3f weighted_velocity = m_pGrid[cell_index]->v * weight;

					//right calculation
					PMatrix<float, 3, 3> term{ dist.X * weighted_velocity, dist.Y * weighted_velocity, dist.Z * weighted_velocity };

					B += term;
					p->v += weighted_velocity;
				}
			}
		}
		p->C = B * 2; // [digit 2 is hyper parametes]
		p->x += p->v * dt;

		p->x.X = FMath::Clamp(p->x.X, 1.f, (float)grid_res - 2);
		p->x.Y = FMath::Clamp(p->x.Y, 1.f, (float)grid_res - 2);
		p->x.Z = FMath::Clamp(p->x.Z, 1.f, (float)grid_res - 2);

		PMatrix<float, 3, 3> Fp_new{ 1.f,1.f,1.f };

		Fp_new += dt * p->C;
		Fs[i] = Fp_new * Fs[i];

		m_pParticles[i] = p;
	}
}

void AMPM3D_Neo_Interaction::UpdateParticles()
{
	Transforms.Empty(TotalParticles);
	ParallelFor(TotalParticles, [&](int32 i)
		{
			Transforms.Add(FTransform(FVec3{ m_pParticles[i]->x.X * 100.f, m_pParticles[i]->x.Y * 100.f, m_pParticles[i]->x.Z * 100.f }));
			
			//FScopeLock Lock(&Mutex);
			InstancedStaticMeshComponent->UpdateInstanceTransform(i, Transforms[i]);
			
		},true);
	/*for (int i = 0; i < TotalParticles; ++i)
	{
		Transforms.Add(FTransform(FVec3{ m_pParticles[i]->x.X * 100.f, m_pParticles[i]->x.Y * 100.f, m_pParticles[i]->x.Z * 100.f }));
		InstancedStaticMeshComponent->UpdateInstanceTransform(i, Transforms[i]);
	}*/
	InstancedStaticMeshComponent->MarkRenderStateDirty();
}

