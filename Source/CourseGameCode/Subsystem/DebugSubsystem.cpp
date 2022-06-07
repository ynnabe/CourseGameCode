// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugSubsystem.h"

bool UDebugSubsystem::IsCategoryEnabled(const FName& CategoryName) const
{
	const bool* bIsEnabled = EnabledCategories.Find(CategoryName);
	return bIsEnabled != nullptr && *bIsEnabled;
}

void UDebugSubsystem::EnableCategory(const FName& CategoryName, bool bIsDrawEnabled)
{
	EnabledCategories.FindOrAdd(CategoryName);
	EnabledCategories[CategoryName] = bIsDrawEnabled;
}
