// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class Json : ModuleRules
	{
		public Json(TargetInfo Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
				}
			); 

			PrivateIncludePaths.AddRange(
				new string[] {
					"Runtime/Json/Private",
				}
			);
		}
	}
}
