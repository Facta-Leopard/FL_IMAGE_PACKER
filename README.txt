# FL_IMAGE_PACKER

**Project:** FL_IMAGE_PACKER  
**Author:** [https://github.com/Facta-Leopard](https://github.com/Facta-Leopard)  
**License:** Creative Commons Attribution 4.0 International (CC BY 4.0)
** This library also makes use of the following third-party component:
- DirectXTex (https://github.com/microsoft/DirectXTex)
- Copyright (c) Microsoft Corporation
- Licensed under the MIT License
- Full License: https://opensource.org/licenses/MIT

---

## Micro Update(2025-04-28)

- **Feature Addition**:
  - Added a feature when using `MF_Attach_ImageSet()` asks the user if overwriting the image is already used.

## Micro Update(2025-04-28)

- **Feature Enhancement**:
  - Increased `extension buffer size to allow up to 15 characters`, `reducing buffer overrun risk`.

## Additional Update(2025-04-27)

- **Feature Enhancement**:
  - Added functionality to **specify a default filename**`(CompressedImage.rsc)` when saving and loading resources.
  - The **default filename is automatically used**, and you may modify it if necessary.

- **Optimization**:
  - To minimize memory access and path restoration overhead when handling default filenames, the following considerations were applied.

### Overhead Comparison Table
| Consideration | Description | Overhead in Previous Method | Overhead in Optimized Method |
|:-------------|:------------|:----------------------------|:-----------------------------|
| 1. Memory Access Cost | Accessing member variables via `this` pointer dereference | 3–5 cycles (pointer dereference) | 1–2 cycles (direct stack offset access) |
| 2. Path Restoration Overhead | wstring copy for path backup and restore | 2 copies + 2 memory reallocations | No copy, 1 memory reallocation (during concatenation) |

> **By applying stack-based caching**, **memory access cost and path restoration overhead have been drastically reduced** when dealing with default filenames.

## Urgent Update(2025-04-27)

- **Bug Fix**:
  - Ensured automatic resource loading during the initialization of `C_ImagePackingManager`.  
  - `MF_Load_All()` is now called in the constructor to immediately populate resources upon manager creation.

- **Optimization**:
  - Removed unnecessary parameter from `MF_Load_All()`.

## Update(2025-04-27)

- **Optimization**:
  - Eliminated unnecessary wstring initialization during load operations.  
  - Replaced default null character initialization with manual memory resizing to reduce CPU overhead.

- **Bug Fix**:
  - Fixed missing serialization/deserialization of `FL_DS_ResourceImageDesc` in `FL_DS_ImageSet`.  
  - Ensured complete save/load consistency for resource metadata.

- **Naming Consistency**:
  - Rechecked and unified temporary identifier naming conventions.  
  - Standardized intermediate variables to match `Naming Convention` style.

- **Context Note**:
  - These issues were identified and corrected during the porting and internal review of this project.

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
- Filename change during save/load will be supported in a future update.

---

# 한국어

**프로젝트:** FL_IMAGE_PACKER  
**작성자:** [https://github.com/Facta-Leopard](https://github.com/Facta-Leopard)  
**라이센스:** Creative Commons Attribution 4.0 International (CC BY 4.0)

---

## 소규모 업데이트(2025-04-28)

- **기능 확장**:
  - 이미지 등록 함수(`MF_Attach_ImageSet()`)를 실행할 때, 이미 이미지가 존재하면 덮어쓰기 여부를 사용자에게 확인하는 기능을 추가했습니다.

## 소규모 업데이트(2025-04-28)

- **개선 사항**:
  - `확장자 최대 길이를 15글자까지 허용하도록 버퍼 크기를 확장`하여, 오버런 가능성을 줄였습니다.

## 추가 업데이트(2025-04-27)

- **기능 확장**:
  - 리소스를 **저장 및 로드**할 때 **기본 파일명**(`CompressedImage.rsc`)을 지정하여 사용할 수 있도록 기능을 추가했습니다.
  - `기본 파일명을 자동으로 사용`하며, 필요 시 수정하시기 바랍니다.

- **최적화**:
  - 기본 파일명 사용 시, 메모리 접근 및 경로 복구 오버헤드를 줄이기 위해 다음 사항을 고려하여 최적화하였습니다.

### 오버헤드 비교 표
| 고려사항 | 설명 | 기존 방식 오버헤드 | 최적화 방식 오버헤드 |
|:--------|:----|:----------------|:----------------|
| 1. 메모리 접근 비용 | this 포인터 역참조 후 멤버 접근 | 3~5 cycles (포인터 역참조 발생) | 1~2 cycles (스택 오프셋 직접 접근) |
| 2. 경로 복구 오버헤드 | 경로 저장 및 복구를 위한 wstring 복사 | 복사 2회 + 메모리 재할당 2회 | 복사 없음, 메모리 재할당 1회 (조합 시) |

> **스택 캐싱 방식을 적용**함으로써, 기본 파일명 처리 시 **메모리 접근 비용과 경로 복구 비용을 모두 극한으로 감소**시켰습니다.

## 긴급 업데이트(2025-04-27)

- **버그 수정**:
  - `C_ImagePackingManager` 객체 생성 시 자동으로 리소스를 로드하도록 변경했습니다.  
  - 생성자 내에서 `MF_Load_All()`을 호출하여 초기화 시 리소스가 즉시 준비됩니다.

- **최적화**:
  - `MF_Load_All()` 함수의 불필요한 인자를 제거했습니다.

## 업데이트(2025-04-27)

- **최적화**:
  - 로드 작업 중 불필요한 wstring 초기화를 제거했습니다.
  - 기본 null 문자 초기화를 제거하고, 메모리 크기만 확보하는 방식으로 변경하여 CPU 오버헤드를 감소시켰습니다.

- **버그 수정**:
  - `FL_DS_ImageSet` 내 `FL_DS_ResourceImageDesc` 멤버의 저장/로드 누락 문제를 수정했습니다.
  - 리소스 메타데이터의 저장 및 복원 일관성을 확보했습니다.

- **명명 규칙 통일**:
  - 임시 식별자 명칭을 `명명규칙(Naming Convention)`에 맞게 통일했습니다.

- **추가 메모**:
  - 본 수정 사항들은 프로젝트 이식 및 내부 점검 과정 중 발견되어 수정되었습니다.

---

## 소개

-FL_IMAGE_PACKER는 기존 이미지파일을 DirectXTex의 ScratchImage 형식의 데이터로 관리하는 정적 라이브러리입니다.  
-COM(Component Object Model) 인터페이스와 DirectX의 플래그 조건을 응용하여 구성했습니다.

## 특징
- ScratchImage 구조를 CPU 메모리 구조체(메타데이터 + 픽셀 블롭)로 변환 및 저장 가능합니다,
- ScratchImage 구조로 간편하게 바꾸기가 가능합니다,(DirectX ID3D11Texture2D로 쉽게 변환 및 사용 가능)
- 전체 입/출력 시스템을 하나의 파일로 관리합니다.(확장자 `.rsc` 파일)
- COM 형식 API 함수로 구성하였습니다.
- WinAPI + DirectXTex를 기반으로 해서 구현하였습니다.(MIT 라이센스 추가 명시함)
- 이 라이브러리는 다음의 제3자 구성 요소를 사용합니다:
  - DirectXTex (https://github.com/microsoft/DirectXTex)
  - Copyright (c) Microsoft Corporation
  - MIT 라이선스에 따라 사용 허가됨
  - 전체 라이선스: https://opensource.org/licenses/MIT

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
- 저장 및 로드 시 파일명 변경 기능 추가

---
