// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"



#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4,FString TypeOfMatch = FString(TEXT("Free4All")));

protected:

	virtual bool Initialize() override;

	//called the level is removed from the world (when we travel to another level)
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

///
/// Callbacks for the custom delegates on the MultiplayerSessionSubsystem
/// 
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	// no UFUNCTION because they are not dynamic delegates
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);


private:

	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();


	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	// the subsystem designed to handle all online session functionality.
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;


	int32 NumPublicConnections{4};
	FString MatchType{TEXT("Free4All")};

};
