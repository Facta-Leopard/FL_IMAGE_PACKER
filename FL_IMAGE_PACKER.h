/******************************************************************************
 *
 * Project: FL_IMAGE_PACKER
 * Author: https://github.com/Facta-Leopard
 * License: Creative Commons Attribution 4.0 International (CC BY 4.0)
 *
 * This header is part of a static resource-handling library developed for use
 * with Scratch-captured image data and metadata-based resource structures.
 * The system applies pointer-based memory layouts, COM-style API encapsulation,
 * and DirectX-style descriptor principles.
 *
 * This library also makes use of the following third-party component:
 *
 * - DirectXTex (https://github.com/microsoft/DirectXTex)
 *   - Copyright (c) Microsoft Corporation
 *   - Licensed under the MIT License
 *   - Full License: https://opensource.org/licenses/MIT
 *
 * You are free to:
 * - Share — copy and redistribute the material in any medium or format
 * - Adapt — remix, transform, and build upon the material for any purpose, even commercially.
 *
 * Under the following terms:
 * - Attribution — You must give appropriate credit (e.g., Facta-Leopard),
 *   provide a link to the license, and indicate if changes were made.
 *
 * This license applies to this file and any header/source files it includes or is included by.
 *
 * Full License: https://creativecommons.org/licenses/by/4.0/
 *
 ******************************************************************************/

#pragma once

#include <Windows.h>
#include <string>
using namespace std;

#include "DirectXTex.h"
#include "DirectXTex.inl"

using namespace DirectX;

// FL_ResourceImageDesc 전용 구분용 플래그
enum FL_E_IMAGETYPE
{
	_IMAGETYPE_PNG,				// PNG 파일용
	_IMAGETYPE_JPG,				// JPG 파일용
	_IMAGETYPE_JPEG,			// JPEG 파일용
	_IMAGETYPE_BMP,				// BMP 파일용

	_IMAGETYPE_END,
};

// C.O.M.(Component Object Model) 개념 적용을 위한 메타데이터 구조체
struct FL_DS_ResourceImageDesc
{
	// 리소스를 저장하거나, 리소스가 있는 디렉토리 주소; 비워둘 경우에는 기본적으로 실행 디렉토리 + (\Resource) 디렉토리 주소를 기반으로 함
	wstring						wstring_Path = L"EMPTY";

	// 저장시 구분하기 위해 구분용 플래그로 사용; 비워둘 경우에는 PNG를 기반으로 함
	FL_E_IMAGETYPE				E_ImageType = _IMAGETYPE_PNG;
};

// 디버깅시에만 저장관련 쓸 수 있음
#ifdef _DEBUG
// 저장용 API 함수
//// 지금까지의 모든 내용 파일로 저장
void FL_Save_AsFile();

//// 경로 상의 파일을 스크래치이미지로 저장하여 내부적으로 변환하여 임시저장하는 함수
void FL_Save_ScratchImage(const wstring& _wstringName, const FL_DS_ResourceImageDesc& _ResourceImageDesc);

//// 경로 상의 파일을 스크래치이미지로 저장하여 내부적으로 변환하여 임시저장하는 함수
void FL_Save_ScratchImage(const string& _stringName, const FL_DS_ResourceImageDesc& _ResourceImageDesc);
#endif // _DEBUG

// 로드용 API 함수
//// 기본버전
HRESULT FL_Load_ScratchImage(const wstring& _wstringName, ScratchImage& _ScratchImage);

//// 오버로딩버전
HRESULT FL_Load_ScratchImage(const string& _stringName, ScratchImage& _ScratchImage);