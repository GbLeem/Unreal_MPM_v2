// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Unreal_MPM_v2 : ModuleRules
{
	private string project_root_path
	{
		get { return Path.Combine(ModuleDirectory, "../.."); }
	}

	public Unreal_MPM_v2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        //my cuda lib
        string custom_cuda_lib_include = "CUDALib/include";
        string custom_cuda_lib_lib = "CUDALib/lib";

        PublicIncludePaths.Add(Path.Combine(project_root_path, custom_cuda_lib_include));
        PublicAdditionalLibraries.Add(Path.Combine(project_root_path, custom_cuda_lib_lib, "cuda_test.lib"));

        //cuda lib
        string cuda_path = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.2";
        string cuda_include = "include";
        string cuda_lib = "lib/x64";

        PublicIncludePaths.Add(Path.Combine(cuda_path, cuda_include));

        //PublicAdditionalLibraries.Add(Path.Combine(cuda_path, cuda_lib, "cudart.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(cuda_path, cuda_lib, "cudart_static.lib"));
    }
}
