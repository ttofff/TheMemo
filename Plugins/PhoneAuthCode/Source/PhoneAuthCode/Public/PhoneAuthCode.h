// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
// Copyright 2020 DongGuoZheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPhoneAuthCodeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
