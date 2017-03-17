// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Internationalization/TextNamespaceUtil.h"

YString TextNamespaceUtil::BuildFullNamespace(const YString& InTextNamespace, const YString& InPackageNamespace, const bool bAlwaysApplyPackageNamespace)
{
	int32 StartMarkerIndex = INDEX_NONE;
	int32 EndMarkerIndex = InTextNamespace.Len() - 1;
	if (InTextNamespace.Len() > 0 && InTextNamespace[EndMarkerIndex] == PackageNamespaceEndMarker && InTextNamespace.FindLastChar(PackageNamespaceStartMarker, StartMarkerIndex))
	{
		YString FullNamespace = InTextNamespace;

		FullNamespace.RemoveAt(StartMarkerIndex + 1, EndMarkerIndex - StartMarkerIndex - 1, /*bAllowShrinking*/false);
		FullNamespace.InsertAt(StartMarkerIndex + 1, InPackageNamespace);

		return FullNamespace;
	}
	else if (bAlwaysApplyPackageNamespace)
	{
		if (InTextNamespace.IsEmpty())
		{
			return YString::Printf(TEXT("%c%s%c"), PackageNamespaceStartMarker, *InPackageNamespace, PackageNamespaceEndMarker);
		}
		else
		{
			return YString::Printf(TEXT("%s %c%s%c"), *InTextNamespace, PackageNamespaceStartMarker, *InPackageNamespace, PackageNamespaceEndMarker);
		}
	}

	return InTextNamespace;
}

YString TextNamespaceUtil::ExtractPackageNamespace(const YString& InTextNamespace)
{
	int32 StartMarkerIndex = INDEX_NONE;
	int32 EndMarkerIndex = InTextNamespace.Len() - 1;
	if (InTextNamespace.Len() > 0 && InTextNamespace[EndMarkerIndex] == PackageNamespaceEndMarker && InTextNamespace.FindLastChar(PackageNamespaceStartMarker, StartMarkerIndex))
	{
		return InTextNamespace.Mid(StartMarkerIndex + 1, EndMarkerIndex - StartMarkerIndex - 1);
	}

	return YString();
}

YString TextNamespaceUtil::StripPackageNamespace(const YString& InTextNamespace)
{
	int32 StartMarkerIndex = INDEX_NONE;
	int32 EndMarkerIndex = InTextNamespace.Len() - 1;
	if (InTextNamespace.Len() > 0 && InTextNamespace[EndMarkerIndex] == PackageNamespaceEndMarker && InTextNamespace.FindLastChar(PackageNamespaceStartMarker, StartMarkerIndex))
	{
		YString StrippedNamespace = InTextNamespace.Left(StartMarkerIndex);
		StrippedNamespace.TrimTrailing();

		return StrippedNamespace;
	}

	return InTextNamespace;
}

#if USE_STABLE_LOCALIZATION_KEYS

YString TextNamespaceUtil::GetPackageNamespace(YArchive& InArchive)
{
	return InArchive.GetLocalizationNamespace();
}

#endif // USE_STABLE_LOCALIZATION_KEYS
