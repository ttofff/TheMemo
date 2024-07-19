// Copyright 2020 DongGuoZheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "HttpModule.h"
#include "InitePost.generated.h"

/**
 * 
 */
UCLASS()
class PHONEAUTHCODE_API UInitePost : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostResult, FString, ReturnMessage);



	UPROPERTY(BlueprintAssignable)
		FPostResult OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FPostResult OnFailed;

	void Event_OnSuccess(FString ReturnMessage);

	
	void Event_OnFailed(FString ReturnMessage);


	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "PostTool")
	static UInitePost* SendActivationCode( const FString& IP, FString PhoneNum);


	UFUNCTION(BlueprintCallable, Category = "PostTool")
	static bool IniteConfig(FString AccessKeyId,FString AccessSecret,FString SignName,FString TemplateCode);

	
	void StartApply(FString URl, FString PhoneNum);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);






	
};
