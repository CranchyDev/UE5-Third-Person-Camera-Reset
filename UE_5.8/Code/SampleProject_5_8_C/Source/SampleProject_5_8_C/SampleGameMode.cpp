// Fill out your copyright notice in the Description page of Project Settings.


#include "SampleGameMode.h"

#include "MyPlayerController.h"
#include "MyCharacter.h"

ASampleGameMode::ASampleGameMode()
{
	PlayerControllerClass = AMyPlayerController::StaticClass();
	DefaultPawnClass = AMyCharacter::StaticClass();
}