#include "MPM3D_NeoHookean_v2.h"

AMPM3D_NeoHookean_v2::AMPM3D_NeoHookean_v2()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));

	SetRootComponent(InstancedStaticMeshComponent);

	InstancedStaticMeshComponent->SetMobility(EComponentMobility::Static);
	InstancedStaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	InstancedStaticMeshComponent->SetGenerateOverlapEvents(false);
}

void AMPM3D_NeoHookean_v2::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnBox();
	//P2G();
	Scatter();

	if (InstancedStaticMeshComponent->GetInstanceCount() == 0)
	{
		Transforms.Empty(NumParticles);

		for (int i = 0; i < NumParticles; ++i)
		{
			FTransform tempValue = FTransform(FVec3(m_pParticles[i]->x.X * 100.f, m_pParticles[i]->x.Y * 100.f, m_pParticles[i]->x.Z * 100.f));
			Transforms.Add(tempValue);
		}
		InstancedStaticMeshComponent->AddInstances(Transforms, false);
	}
}

void AMPM3D_NeoHookean_v2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Simulate();

	ClearGrid();
	P2G();
	UpdateGrid();
	G2P();
	UpdateParticles();
}


void AMPM3D_NeoHookean_v2::SpawnBox()
{
	const float spacing = 0.5f;
	const int32 box_x = 8;
	const int32 box_y = 8;
	const int32 box_z = 8;
	const float sx = grid_res / 2.f;
	const float sy = grid_res / 2.f;
	const float sz = grid_res / 2.f;
	
	for (float i = -box_x; i < box_x; i += spacing) //-16+16
	{
		for (float j = -box_y; j < box_y; j += spacing)
		{
			for (float k = -box_z; k < box_z; k += spacing)
			{
				TempPositions.Add(FVec3f(sx + i, sy + j, sz + k));
			}
		}
	}

	NumParticles = TempPositions.Num();
	m_pParticles.Empty(NumParticles);
	Fs.Empty(NumParticles);

	
	//ParallelFor(NumParticles, [&](int32 i)
	for (int32 i = 0; i < NumParticles; ++i)
	{
		//MutexForParticle.Lock();
		Particle* p = new Particle();
		p->x = TempPositions[i];
		p->v = { 0.f, 0.f, 0.f };
		p->C;
		p->mass = 1.f;

		m_pParticles.Add(p);
		//MutexForParticle.Unlock();
		//Chaos::PMatrix<float, 3, 3>(1, 1, 1);// temp(1, 1, 1);
		Fs.Add(FMatrix33(1, 1, 1));
	}

	m_pGrid.Empty(NumCells);

	/*ParallelFor(NumCells, [&](int32 i)
		{
			Mutex.Lock();
			Cell* cell = new Cell();
			cell->v = { 0.f, 0.f, 0.f };
			m_pGrid.Add(cell); 
			Mutex.Unlock();
		},true);*/

	for (int i = 0; i < NumCells; ++i)
	{
		Cell* cell = new Cell();
		cell->v = { 0.f, 0.f, 0.f };
		m_pGrid.Add(cell);
	}
}

void AMPM3D_NeoHookean_v2::Scatter()
{
	P2G();
	for (int32 i = 0; i < NumParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		FVec3f cell_idx = { (float)floor(p->x.X), (float)floor(p->x.Y) ,(float)floor(p->x.Z) };
		FVec3f cell_diff = { (p->x.X - cell_idx.X - 0.5f), (p->x.Y - cell_idx.Y - 0.5f), (p->x.Z - cell_idx.Z - 0.5f) };

		weights.Empty(3);
		//weights.
		weights.Add({ 0.5f * (float)pow(0.5f - cell_diff.X, 2), 0.5f * (float)pow(0.5f - cell_diff.Y, 2),0.5f * (float)pow(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - (float)pow(cell_diff.X, 2), 0.75f - (float)pow(cell_diff.Y, 2), 0.75f - (float)pow(cell_diff.Z, 2) });
		weights.Add({ 0.5f * (float)pow(0.5f + cell_diff.X, 2), 0.5f * (float)pow(0.5f + cell_diff.Y, 2), 0.5f * (float)pow(0.5f + cell_diff.Z, 2) });

		float density = 0.f;

		for (int32 gx = 0; gx < 3; ++gx)
		{
			for (int32 gy = 0; gy < 3; ++gy)
			{
				for (int32 gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					int32 cell_index = ((int)(cell_idx.X) + gx - 1) * grid_res * grid_res + ((int)cell_idx.Y + gy - 1) * grid_res + ((int)cell_idx.Z + gz - 1);
					density += m_pGrid[cell_index]->mass * weight;
				}
			}
		}
		float volume = p->mass / density;
		p->volume_0 = volume;

		m_pParticles[i] = p;
	}
}

void AMPM3D_NeoHookean_v2::ClearGrid()
{
	/*ParallelFor(NumCells, [&](int32 i)
		{
			Mutex.Lock();
			Cell* cell = m_pGrid[i];

			cell->mass = 0;
			cell->v = { 0.f,0.f,0.f };

			m_pGrid[i] = cell;
			Mutex.Unlock();
	}, true);*/

	for (int32 i = 0; i < NumCells; ++i)
	{
		Cell* cell = m_pGrid[i];

		cell->mass = 0;
		cell->v = { 0.f,0.f,0.f };

		m_pGrid[i] = cell;
	}
}

void AMPM3D_NeoHookean_v2::P2G()
{
	for (int i = 0; i < NumParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		FMatrix33 stress(0, 0, 0, 0, 0, 0, 0, 0, 0);

		FMatrix33 F = Fs[i];

		float J = F.Determinant();

		float volume = p->volume_0 * J;

		//Chaos::FMatrix33
		FMatrix33 F_T = F.GetTransposed();
		FMatrix33 F_inv_T = F_T.InverseFast();
		FMatrix33 F_minus_F_inv_T = F - F_inv_T;

		FMatrix33 P_term_0 = elastic_mu * F_minus_F_inv_T;
		FMatrix33 P_term_1 = elastic_lambda * log(J) * F_inv_T;
		FMatrix33 P = P_term_0 + P_term_1;
		
		stress = (1.f / J) * (P * F_T);

		FMatrix33 eq_16_term_0 = -volume * 2 * stress * dt; // [digit 2 is hyper parameter]

		TVec3<int> cell_idx(p->x.X, p->x.Y, p->x.Z);
		FVec3f cell_diff(p->x.X - cell_idx.X - 0.5f, p->x.Y - cell_idx.Y - 0.5f, p->x.Z - cell_idx.Z - 0.5f);

		weights.Empty(3);
		weights.Add({ 0.5f * (float)pow(0.5f - cell_diff.X, 2), 0.5f * (float)pow(0.5f - cell_diff.Y, 2),0.5f * (float)pow(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - (float)pow(cell_diff.X, 2), 0.75f - (float)pow(cell_diff.Y, 2), 0.75f - (float)pow(cell_diff.Z, 2) });
		weights.Add({ 0.5f * (float)pow(0.5f + cell_diff.X, 2), 0.5f * (float)pow(0.5f + cell_diff.Y, 2), 0.5f * (float)pow(0.5f + cell_diff.Z, 2) });

		for (int gx = 0; gx < 3; ++gx)
		{
			for (int gy = 0; gy < 3; ++gy)
			{
				for (int gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					TVec3<int> cell_x(cell_idx.X + gx - 1, cell_idx.Y + gy - 1, cell_idx.Z + gz - 1);
					FVec3f cell_dist(cell_x.X - p->x.X + 0.5f, cell_x.Y - p->x.Y + 0.5f, cell_x.Z - p->x.Z + 0.5f);
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



void AMPM3D_NeoHookean_v2::UpdateGrid()
{
	/*ParallelFor(NumCells, [&](int32 i)
	{		
		Mutex.Lock();
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
			Mutex.Unlock();

		}	
	}, EParallelForFlags::ForceSingleThread);*/

	for (int32 i = 0; i < NumCells; ++i)
	{
		Cell* cell = m_pGrid[i];

		if (cell->mass > 0)
		{
			cell->v /= cell->mass;
			cell->v += dt * FVec3f(0, 0, gravity);

			int32 x = i / (grid_res * grid_res);
			int32 y = (i % (grid_res * grid_res)) / grid_res;
			int32 z = i % grid_res;

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
	}
	
}

void AMPM3D_NeoHookean_v2::G2P()
{
	for (int32 i = 0; i < NumParticles; ++i)
	{
		Particle* p = m_pParticles[i];

		p->v = { 0.f, 0.f, 0.f };

		TVec3<int> cell_idx{ (int)p->x.X, (int)p->x.Y, (int)p->x.Z };
		FVec3f cell_diff{ p->x.X - cell_idx.X - 0.5f, p->x.Y - cell_idx.Y - 0.5f, p->x.Z - cell_idx.Z - 0.5f };

		weights.Empty(3);
		weights.Add({ 0.5f * (float)pow(0.5f - cell_diff.X, 2), 0.5f * (float)pow(0.5f - cell_diff.Y, 2), 0.5f * (float)pow(0.5f - cell_diff.Z, 2) });
		weights.Add({ 0.75f - (float)pow(cell_diff.X, 2), 0.75f - (float)pow(cell_diff.Y, 2), 0.75f - (float)pow(cell_diff.Z, 2) });
		weights.Add({ 0.5f * (float)pow(0.5f + cell_diff.X, 2), 0.5f * (float)pow(0.5f + cell_diff.Y, 2), 0.5f * (float)pow(0.5f + cell_diff.Z, 2) });

		FMatrix33 B(0.f, 0.f, 0.f);

		for (int32 gx = 0; gx < 3; ++gx)
		{
			for (int32 gy = 0; gy < 3; ++gy)
			{
				for (int32 gz = 0; gz < 3; ++gz)
				{
					float weight = weights[gx].X * weights[gy].Y * weights[gz].Z;

					TVec3<int> cell_x{ cell_idx.X + gx - 1, cell_idx.Y + gy - 1, cell_idx.Z + gz - 1 };
					int32 cell_index = (int)cell_x.X * grid_res * grid_res + (int)cell_x.Y * grid_res + (int)cell_x.Z;

					FVec3f dist = { (cell_x.X - p->x.X) + 0.5f, (cell_x.Y - p->x.Y) + 0.5f, (cell_x.Z - p->x.Z) + 0.5f };
					FVec3f weighted_velocity = m_pGrid[cell_index]->v * weight;

					//right calculation
					//PMatrix<float, 3, 3> term(weighted_velocity * dist.X, weighted_velocity * dist.Y, weighted_velocity * dist.Z);
					FMatrix33 term;
					term.M[0][0] = (weighted_velocity.X * dist.X);
					term.M[1][0] = (weighted_velocity.Y * dist.X);
					term.M[2][0] = (weighted_velocity.Z * dist.X);

					term.M[0][1] = (weighted_velocity.X * dist.Y);
					term.M[1][1] = (weighted_velocity.Y * dist.Y);
					term.M[2][1] = (weighted_velocity.Z * dist.Y);

					term.M[0][2] = (weighted_velocity.X * dist.Z);
					term.M[1][2] = (weighted_velocity.Y * dist.Z);
					term.M[2][2] = (weighted_velocity.Z * dist.Z);

					B += term;
					p->v += weighted_velocity;
				}
			}
		}
		p->C = B * 2; // [digit 2 is hyper parametes]
		p->x += p->v * dt;

		p->x.X = FMath::Clamp(p->x.X, 1, grid_res - 2);
		p->x.Y = FMath::Clamp(p->x.Y, 1, grid_res - 2);
		p->x.Z = FMath::Clamp(p->x.Z, 1, grid_res - 2);

		FMatrix33 Fp_new(1, 0, 0, 0, 1, 0, 0, 0, 1);

		Fp_new += dt * p->C;
		Fs[i] = Fp_new * Fs[i];

		m_pParticles[i] = p;
	}
}

//void AMPM3D_NeoHookean_v2::Simulate()
//{
//	ClearGrid();
//	P2G();
//	UpdateGrid();
//	G2P();
//}

void AMPM3D_NeoHookean_v2::UpdateParticles()
{
	Transforms.Empty(NumParticles);

	for (int i = 0; i < NumParticles; ++i)
	{
		FTransform tempValue = FTransform(FVec3(m_pParticles[i]->x.X * 100.f, m_pParticles[i]->x.Y * 100.f, m_pParticles[i]->x.Z * 100.f));
		Transforms.Add(tempValue);
		InstancedStaticMeshComponent->UpdateInstanceTransform(i, Transforms[i]);
	}
	InstancedStaticMeshComponent->MarkRenderStateDirty();
}
