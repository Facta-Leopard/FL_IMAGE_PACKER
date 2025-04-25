# FL_IMAGE_PACKER

**Project:** FL_IMAGE_PACKER  
**Author:** [https://github.com/Facta-Leopard](https://github.com/Facta-Leopard)  
**License:** Creative Commons Attribution 4.0 International (CC BY 4.0)

---

## About

- FL_IMAGE_PACKER is a static library for handling image resources captured from Scratch.
- It manages image data and metadata efficiently, focusing on a COM-style (Component Object Model) design and DirectX-style descriptor architecture.

## Features
- ScratchImage structures are serialized into simple CPU memory structures (metadata + pixel blob).
- Supports conversion back into ScratchImage objects.
- Binary save/load system for entire collections (with `.rsc` extension).
- COM-style function encapsulation and separation.
- WinAPI + DirectXTex based.

## API Overview

### Save Flow
- Convert to ScratchImage → store using `FL_Save_ScratchImage()` → batch-save to a `.rsc` file with `FL_Save_AsFile()` when needed.

- **Note: Save-related APIs are only available in `_DEBUG` mode.**

#### Save APIs (Debug Mode Only)
- `void FL_Save_AsFile();`  — Saves all currently stored images into a `.rsc` file.
- `void FL_Save_ScratchImage(const wstring& name, const FL_DS_ResourceImageDesc& desc);`
- `void FL_Save_ScratchImage(const string& name, const FL_DS_ResourceImageDesc& desc);`

### Load Flow
- During runtime, call `FL_Load_ScratchImage()` to restore images into memory → create DirectX resources from the loaded ScratchImages.

#### Load APIs
- `HRESULT FL_Load_ScratchImage(const wstring& name, ScratchImage& out);`
- `HRESULT FL_Load_ScratchImage(const string& name, ScratchImage& out);`

## Notes
- Only non-compressed WIC-supported formats are currently handled (PNG, JPG, JPEG, BMP).
- Resource management relies on STL containers.
- Default save path: `./Resource/`

## Future Improvements
- Encryption/obfuscation of `.rsc` files.
- Adding checksum validation.

---

# 한국어

**프로젝트:** FL_IMAGE_PACKER  
**작성자:** [https://github.com/Facta-Leopard](https://github.com/Facta-Leopard)  
**라이센스:** Creative Commons Attribution 4.0 International (CC BY 4.0)

---

## 소개

-FL_IMAGE_PACKER는 기존 이미지파일을 DirectXTex의 ScratchImage 형식의 더이터로 관리하는 정적 라이브러리입니다.  
-COM(Component Object Model) 인터페이스와 DirectX 형태 데스크립터 조건을 적용하여 구성했습니다.

## 특징
- ScratchImage 구조를 CPU 메모리 구조체(메타데이터 + 핏셀 블롭)로 변환 및 저장 가능합니다,
- ScratchImage 구조로 간편하게 바꾸기가 가능합니다,(DirectX ID3D11Texture2D로 쉽게 변환 및 사용 가능)
- 전체 입/출력 시스템을 하나의 파일로 관리합니다.(확장자 `.rsc` 파일)
- COM 형식 API 함수 형태 사용하였습니다.
- WinAPI + DirectXTex를 기반으로 해서 구현하였습니다.

## API 사용법

### Save 사용법
- ScratchImage로 변환하고 `FL_Save_ScratchImage()`로 저장 → 필요할 때 `FL_Save_AsFile()`로 전체를 `.rsc` 파일로 저장.

- **※ 주의: Save 관련 API는 `_DEBUG`에서만 사용할 수 있습니다.**

#### 저장 API 함수
- `void FL_Save_AsFile();`
> 현재 메모리에 등록된 모든 이미지를 `.rsc` 파일로 저장합니다.

- `void FL_Save_ScratchImage(const wstring& name, const FL_DS_ResourceImageDesc& desc);`
> 주어진 파일 경로(name)에서 이미지를 읽어 ScratchImage로 변환한 뒤 메모리에 임시 저장합니다.

- `void FL_Save_ScratchImage(const string& name, const FL_DS_ResourceImageDesc& desc);`
> (string 버전) 위 함수의 오버로딩입니다. 문자열 타입만 다릅니다.

### Load 사용법
- 프로그램 실행 시 `FL_Load_ScratchImage()`를 호출해 필요한 이미지를 메모리로 복원 → 이후 ScratchImage 기반으로 DirectX 리소스 생성 등 사용 가능

#### 로드 API 함수
- `HRESULT FL_Load_ScratchImage(const wstring& name, ScratchImage& out);`
> 지정한 이름(name)으로 저장된 이미지를 메모리에서 찾아 ScratchImage로 복원합니다.

- `HRESULT FL_Load_ScratchImage(const string& name, ScratchImage& out);`
> (string 버전) 위 함수의 오버로딩입니다. 문자열 타입만 다릅니다.

## 비고
- 현재는 WIC(PNG, JPG, JPEG, BMP)파일만 가능합니다.
- 리소스 관리는 내부 STL 컨테이너를 사용합니다.
- 저장 및 로드하는 경로는 기본적으로 솔루션 디렉토리 + `./Resource/`입니다.

---

## 업데이트 계획
- .rsc 파일 크래킹 방지를 위한 암호화
- 관리 로직 변경(싱글턴 등)

---
