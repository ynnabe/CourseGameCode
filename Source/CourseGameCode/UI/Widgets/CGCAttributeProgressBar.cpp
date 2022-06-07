// Fill out your copyright notice in the Description page of Project Settings.


#include "CGCAttributeProgressBar.h"

#include "Components/ProgressBar.h"

void UCGCAttributeProgressBar::SetProgressPercantage(float Percentage)
{
	Healthbar->SetPercent(Percentage);
}
