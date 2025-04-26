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

#include "FL_IMAGE_PACKER.h"

#include <map>

// 스크래치 이미지로 환원시 사용할 정보
struct FL_DS_ImageEntry
{
	size_t				SDK_M_Width;				// 높이정보
	size_t				SDK_M_Height;				// 길이정보
	DXGI_FORMAT			DX_M_ImageFormat;			// 이미지 포맷형식
	size_t				SDK_M_RowPitch;				// 한 줄당 픽셀이 차지하는 크기
	size_t				SDK_M_SlicePitch;			// 전체 이미지 크기(SDK_M_RowPitch * SDK_M_Height)
	size_t				SDK_M_OffsetInBlob;			// Offset 정보
};

// 리소스 패킹전용 구조체
struct FL_DS_CPU_Image
{
	TexMetadata					M_MetaData;			// 이미지 파일이 갖고 있는 메타데이터; 밈맵 증 정보가 들어있음; DirectX 전용
	vector<FL_DS_ImageEntry>       STL_M_Entry;		// 스크래치 이미지로 환원시 사용할 정보들
	vector<uint8_t>             M_PixelBlob;		// 이미지에 관한 전체정보(1차원 정보)로, 모든 정보가 한 줄로 담김
};

struct FL_DS_ImageSet
{
	FL_DS_CPU_Image					M_CPUImage;
	FL_DS_ResourceImageDesc			M_Desc;
};

class C_ImagePackingManager
{
public:
	C_ImagePackingManager()
		: STL_M_ImageSet{}
	{
		// .rsc 파일이 저장되어 있는 기본 위치 설정
		wchar_t wchar_T_Path_s[255] = {};
		wstring wstring_BasicPath = {};
		GetCurrentDirectory(255, wchar_T_Path_s);
		wcscat_s(wchar_T_Path_s, 255, L"\\Resource\\");
		wstring_BasicPath = wchar_T_Path_s;
	}

public:
	C_ImagePackingManager(const C_ImagePackingManager& _Origin) = delete;

public:
	~C_ImagePackingManager()
	{
		for (std::pair<const wstring, FL_DS_ImageSet*>& T_Pair : STL_M_ImageSet)
		{
			if (nullptr != T_Pair.second)		// 방어코드
			{
				delete T_Pair.second;
				T_Pair.second = nullptr;
			}
		}

		STL_M_ImageSet.clear();			// map 자체도 비우기
	}

protected:
	map<wstring, FL_DS_ImageSet*>		STL_M_ImageSet;			// map<wstring, FL_DS_ImageSet*>; 데이터를 저장할 STL컨테이너
	wstring							wstring_BasicPath;		// wstring; 기본적인 디렉토리 주소; 실행 디렉토리 + (\Resource) 디렉토리 주소 설정

public:
	void MF_Save_All()
	{
		// 향후, 난독화 및 분기 등으로, 메모리 덤프 등으로 리버스 엔지니어링하는 것을 막는 것도 고려해보자
		
		FILE* SDK_T_File = nullptr;
		_wfopen_s(&SDK_T_File, wstring_BasicPath.c_str(), L"wb");

		// 향후, 조건문 부분을 _wfopen_s 반환값으로 하면 오버헤드가 더 적을 것 같긴한데, 10시간째 씨름중이라 나중에 다시 생각해보자
		if (nullptr == SDK_T_File)
		{
			MessageBox(nullptr, L"_wfopen_s() Failed", L"_wfopen_s() Must Check", MB_OK | MB_ICONINFORMATION);
		}

		size_t T_MapSize = STL_M_ImageSet.size();
		fwrite(&T_MapSize, sizeof(size_t), 1, SDK_T_File);

		for (const auto& pair : STL_M_ImageSet)
		{
			const wstring& key = pair.first;
			const size_t keyLength = key.length();

			fwrite(&keyLength, sizeof(size_t), 1, SDK_T_File);
			fwrite(key.c_str(), sizeof(wchar_t), keyLength, SDK_T_File);


			const FL_DS_ImageSet* T_ImageSet = pair.second;
			const FL_DS_CPU_Image& DS_T_CpuImage = T_ImageSet->M_CPUImage;

			fwrite(&DS_T_CpuImage.M_MetaData, sizeof(TexMetadata), 1, SDK_T_File);

			const size_t entryCount = DS_T_CpuImage.STL_M_Entry.size();
			fwrite(&entryCount, sizeof(size_t), 1, SDK_T_File);
			fwrite(DS_T_CpuImage.STL_M_Entry.data(), sizeof(FL_DS_ImageEntry), entryCount, SDK_T_File);

			const size_t pixelSize = DS_T_CpuImage.M_PixelBlob.size();
			fwrite(&pixelSize, sizeof(size_t), 1, SDK_T_File);
			fwrite(DS_T_CpuImage.M_PixelBlob.data(), 1, pixelSize, SDK_T_File);
		}

		fclose(SDK_T_File);
	}

	void MF_Load_All(const wstring& _wstringName)
	{
		// 향후, 난독화 및 분기 등으로, 메모리 덤프 등으로 리버스 엔지니어링하는 것을 막는 것도 고려해보자

		FILE* SDK_T_File = nullptr;
		_wfopen_s(&SDK_T_File, wstring_BasicPath.c_str(), L"rb");

		// 향후, 조건문 부분을 _wfopen_s 반환값으로 하면 오버헤드가 더 적을 것 같긴한데, 10시간째 씨름중이라 나중에 다시 생각해보자
		if (nullptr == SDK_T_File)
		{
			MessageBox(nullptr, L"_wfopen_s() Failed", L"_wfopen_s() Must Check", MB_OK | MB_ICONINFORMATION);
		}

		size_t T_MapSize = 0;
		fread(&T_MapSize, sizeof(size_t), 1, SDK_T_File);

		for (size_t i = 0; i < T_MapSize; ++i)
		{
			size_t keyLength = 0;
			fread(&keyLength, sizeof(size_t), 1, SDK_T_File);

			wstring key(keyLength, L'\0');
			fread(&key[0], sizeof(wchar_t), keyLength, SDK_T_File);

			auto* T_ImageSet = new FL_DS_ImageSet();
			fread(&T_ImageSet->M_CPUImage.M_MetaData, sizeof(TexMetadata), 1, SDK_T_File);

			size_t entryCount = 0;
			fread(&entryCount, sizeof(size_t), 1, SDK_T_File);
			T_ImageSet->M_CPUImage.STL_M_Entry.resize(entryCount);
			fread(T_ImageSet->M_CPUImage.STL_M_Entry.data(), sizeof(FL_DS_ImageEntry), entryCount, SDK_T_File);

			size_t pixelSize = 0;
			fread(&pixelSize, sizeof(size_t), 1, SDK_T_File);
			T_ImageSet->M_CPUImage.M_PixelBlob.resize(pixelSize);
			fread(T_ImageSet->M_CPUImage.M_PixelBlob.data(), 1, pixelSize, SDK_T_File);

			STL_M_ImageSet.insert({ key, T_ImageSet });
		}

		fclose(SDK_T_File);
	}

	FL_DS_ImageSet* MF_FindFromVectorData(const wstring& _wstringName)
	{
		map<wstring, FL_DS_ImageSet*>::iterator T_Iterator = STL_M_ImageSet.find(_wstringName);

		if (STL_M_ImageSet.end() != T_Iterator)
		{
			return (*T_Iterator).second;
		}
		else
		{
			return nullptr;
		}
	}

	FL_DS_ImageSet* MF_FindFromVectorData(const string& _stringName)
	{
		const wstring wstring_T_Name = MF_ConvertStringToWString_WinAPI(_stringName);

		// 래핑
		return MF_FindFromVectorData(wstring_T_Name);
	}

	void MF_Attach_ImageSet(const wstring& _wstringName, FL_DS_ImageSet* _ImageSet)
	{
		map<wstring, FL_DS_ImageSet*>::iterator T_Iterator = STL_M_ImageSet.find(_wstringName);
		if (T_Iterator != STL_M_ImageSet.end())
		{
			delete T_Iterator->second;

			STL_M_ImageSet[_wstringName] = _ImageSet;
		}
		else
		{
			STL_M_ImageSet.insert(make_pair(_wstringName, _ImageSet));
		}
	}

	// 모듈용 함수
public:
	// 스크래치 이미지를 리소스 관리용 구조체로 변환하는 함수; COM(Component Object Model) 방식
	HRESULT MF_Store_ScratchImage_To_CPUImage(const ScratchImage& _ScratchImage, FL_DS_ImageSet& _ImageSet)
	{
		FL_DS_CPU_Image T_CPU_Image = _ImageSet.M_CPUImage;
		const Image* P_ImageArray = _ScratchImage.GetImages();
		size_t Count = _ScratchImage.GetImageCount();
		const uint8_t* P_PixelsBase = _ScratchImage.GetPixels();
		size_t TotalSize = _ScratchImage.GetPixelsSize();

		if ((nullptr == P_ImageArray) || (nullptr == P_PixelsBase) || (Count == 0) || (TotalSize == 0))
		{
			return E_FAIL;
		}
		
		T_CPU_Image.M_MetaData = _ScratchImage.GetMetadata();
		T_CPU_Image.M_PixelBlob.resize(TotalSize);

		// 실제 메모리 버퍼 복사
		memcpy(T_CPU_Image.M_PixelBlob.data(), P_PixelsBase, TotalSize);

		T_CPU_Image.STL_M_Entry.clear();
		for (size_t i = 0; i < Count; ++i)
		{
			const Image& T_Image = P_ImageArray[i];
			FL_DS_ImageEntry Entry = {};
			Entry.SDK_M_Width = T_Image.width;
			Entry.SDK_M_Height = T_Image.height;
			Entry.DX_M_ImageFormat = T_Image.format;
			Entry.SDK_M_RowPitch = T_Image.rowPitch;
			Entry.SDK_M_SlicePitch = T_Image.slicePitch;
			Entry.SDK_M_OffsetInBlob = static_cast<size_t>(T_Image.pixels - P_PixelsBase);

			T_CPU_Image.STL_M_Entry.push_back(Entry);
		}

		// 자꾸 이부분 실수하는 듯; STL 컨테이너는 기본적으로 얉은 복사니까 강제적으로 깊은 복사를 시켜야 함
		_ImageSet.M_CPUImage = std::move(T_CPU_Image);

		return S_OK;
	}

	// 리소스 관리용 구조체를 스크래치 이미지로 변환하는 함수; COM(Component Object Model) 방식
	HRESULT MF_Restore_ScratchImage(const FL_DS_CPU_Image& _Source_CPU_Image, ScratchImage& _Out_ScratchImage_Out)
	{
		if (FAILED(_Out_ScratchImage_Out.Initialize(_Source_CPU_Image.M_MetaData)))		// 방어코드
		{
			return E_FAIL;
		}

		const Image* P_ImageArray = _Out_ScratchImage_Out.GetImages();
		size_t Count = _Out_ScratchImage_Out.GetImageCount();
		uint8_t* P_PixelsBase = _Out_ScratchImage_Out.GetPixels();
		size_t TotalSize = _Out_ScratchImage_Out.GetPixelsSize();

		if (nullptr == P_ImageArray || nullptr == P_PixelsBase || Count != _Source_CPU_Image.STL_M_Entry.size())
		{
			return E_FAIL;
		}

		if (TotalSize > _Source_CPU_Image.M_PixelBlob.size())
		{
			return E_FAIL;
		}

		memcpy(P_PixelsBase, _Source_CPU_Image.M_PixelBlob.data(), TotalSize);

		return S_OK;
	}

	// string을 wstring으로 변환하는 함수
	wstring MF_ConvertStringToWString_WinAPI(const string& _string)
	{
		// 변환에 필요한 문자열 개수 계산
		int wstring_T_Size = MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, nullptr, 0);

		// 문자열 개수만큼 wstring 버퍼 생성
		wstring wstring(wstring_T_Size, 0);

		// 변환
		MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, &wstring[0], wstring_T_Size);

		return wstring;
	}

};

// 매니저 객체 생성
class C_ImagePackingManager C_ImageManager;

// 디버깅 시에만 저장용 API함수를 쓸 수 있도록 함
#ifdef _DEBUG
void FL_Save_AsFile()
{
	C_ImageManager.MF_Save_All();
}

void FL_Save_ScratchImage(const wstring& _wstringName, const FL_DS_ResourceImageDesc& _ResourceImageDesc)
{
	// 저장할 임시용 구조체 초기화
	FL_DS_ImageSet T_ImageSet = {};

	// 임시용 구조체에 이미지리소스 관련 메타데이터; 유의! Scratch용 메타데이터와 헷갈리지 말 것!
	T_ImageSet.M_Desc = _ResourceImageDesc;

	// 이미지를 긇어와서 쓸 임시용 구조체 초기화 
	ScratchImage T_ScratchImage = {};

	// 이미지를 긇어올 경로 관련 wchar 문자영 초기화
	wchar_t wchar_T_Path_s[255] = {};

	wstring wstring_T_Path = {};

	// 솔루션 디렉토리 주소 불러오기
	if (L"EMPTY" == T_ImageSet.M_Desc.wstring_Path)
	{
		GetCurrentDirectory(255, wchar_T_Path_s);

		wcscat_s(wchar_T_Path_s, 255, L"\\Resource\\");

		wstring_T_Path = wchar_T_Path_s + _wstringName;
	}
	else
	{
		wstring_T_Path = T_ImageSet.M_Desc.wstring_Path + _wstringName;
	}

	// 향후, WIC말고 다른 포맷을 추가할 경우 이 부분 개선고려
	wchar_t wchar_T_Extension[4] = {};
	_wsplitpath_s(wstring_T_Path.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, wchar_T_Extension, 16);
	wstring wstring_T_Extension = wchar_T_Extension;

	// 확장자에 따른 스크래칭 구분(WIC만 가능)
	if ((wstring_T_Extension == L".png") || (wstring_T_Extension == L".PNG")
		|| (wstring_T_Extension == L".jpg") || (wstring_T_Extension == L".JPG")
		|| (wstring_T_Extension == L".jpeg") || (wstring_T_Extension == L".JPEG")
		|| (wstring_T_Extension == L".bmp") || (wstring_T_Extension == L".BMP"))
	{
		if (FAILED(LoadFromWICFile(wstring_T_Path.c_str(), WIC_FLAGS_NONE, nullptr, T_ScratchImage)))
		{
			MessageBox(nullptr, L"LoadFromWICFile Failed", L"File Extension Is Not WIC", MB_OK | MB_ICONINFORMATION);
		}
	}

	HRESULT T_Result = C_ImageManager.MF_Store_ScratchImage_To_CPUImage(T_ScratchImage, T_ImageSet);

	if (FAILED(T_Result))
	{
		MessageBox(nullptr, L"ScratchImage Converting Failed", L"C_ImageManager.MF_Store_ScratchImage_To_CPUImage() Must Check", MB_OK | MB_ICONINFORMATION);
	}

	FL_DS_ImageSet* P_T_ImageSet = &T_ImageSet;

	C_ImageManager.MF_Attach_ImageSet(_wstringName, P_T_ImageSet);
}

void FL_Save_ScratchImage(const string& _stringName, const FL_DS_ResourceImageDesc& _ResourceImageDesc)
{
	wstring wstring_T_Name = C_ImageManager.MF_ConvertStringToWString_WinAPI(_stringName);

	// 래핑
	FL_Save_ScratchImage(wstring_T_Name, _ResourceImageDesc);
}
#endif // _DEBUG

HRESULT FL_Load_ScratchImage(const wstring& _wstringName, ScratchImage& _ScratchImage)
{
	FL_DS_ImageSet* T_ImageSet = C_ImageManager.MF_FindFromVectorData(_wstringName);

	if (nullptr != T_ImageSet)
	{
		FL_DS_ResourceImageDesc T_ResourceImageDesc = T_ImageSet->M_Desc;

		// 모듈함수 이용
		HRESULT T_Result = C_ImageManager.MF_Restore_ScratchImage(T_ImageSet->M_CPUImage, _ScratchImage);

		if (FAILED(T_Result))
		{
			return E_FAIL;
		}
		else
		{
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT FL_Load_ScratchImage(const string& _stringName, ScratchImage& _ScratchImage)
{
	const wstring wstring_T_Name = C_ImageManager.MF_ConvertStringToWString_WinAPI(_stringName);

	// 래핑
	return FL_Load_ScratchImage(wstring_T_Name, _ScratchImage);
}


