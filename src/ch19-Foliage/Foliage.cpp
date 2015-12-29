#include "Foliage.h"
#include "DirectXTK/DDSTextureLoader.h"
#include <ctime>

namespace byhj
{
   void Foliage::Init(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
   {
	   m_FoliageType.resize(m_FoliageCount);
	   m_InstanceType.resize(m_FoliageCount);

	   gen_pos();

	   init_buffer(pD3D11Device, pD3D11DeviceContext);
	   init_shader(pD3D11Device, hWnd);
	   init_texture(pD3D11Device);
   }

   void Foliage::Update(ID3D11DeviceContext *pD3D11DeviceContext)
   {
	   if (m_WindRotation == 1)
	   {
		   m_WindRotation += 0.1f;
		   if (m_WindRotation > 10.0f)
		   {
			   m_WindDirection = 2;
		   }
	   }
	   else
	   {
		   m_WindRotation -= 0.1f;
		   if (m_WindRotation < -10.0f)
		   {
			   m_WindDirection = 1;
		   }
	   }

	   XMFLOAT3 modelPos;
	   XMFLOAT3 cameraPos;
	   // Load the instance buffer with the updated locations.
	   for (int i = 0; i < m_FoliageCount; i++)
	   {
		   // Get the position of this piece of foliage.
		   modelPos.x = m_FoliageType[i].x;
		   modelPos.y = -0.1f;
		   modelPos.z = m_FoliageType[i].z;

		   // Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
		   auto angle = atan2(modelPos.x - cameraPos.x, modelPos.z - cameraPos.z) * (180.0 / XM_PI);

		   // Convert rotation into radians.
		   rotation = (float)angle * 0.0174532925f;

		   // Setup the X rotation of the billboard.
		   D3DXMatrixRotationY(&rotateMatrix, rotation);

		   // Get the wind rotation for the foliage.
		   m_WindRotation =   m_WindRotation * 0.0174532925f;

		   // Setup the wind rotation.
		   D3DXMatrixRotationX(&rotateMatrix2,   m_WindRotation);

		   // Setup the translation matrix.
		   D3DXMatrixTranslation(&translationMatrix, modelPos.x, modelPos.y, modelPos.z);

		   // Create the final matrix and store it in the instances array.
		   D3DXMatrixMultiply(&finalMatrix, &rotateMatrix, &rotateMatrix2);
		   D3DXMatrixMultiply(&m_Instances[i].matrix, &finalMatrix, &translationMatrix);
	   }

	   D3D11_MAPPED_SUBRESOURCE mappedResource;
	   // Lock the instance buffer so it can be written to.
	   pD3D11DeviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	   InstanceType* instancesPtr;
	   // Get a pointer to the data in the instance buffer.
	   instancesPtr = (InstanceType*)mappedResource.pData;

	   // Copy the instances array into the instance buffer.
	   memcpy(instancesPtr,  &m_InstanceType[0], (sizeof(InstanceType) * m_FoliageCount));

	   // Unlock the instance buffer.
	   pD3D11DeviceContext->Unmap(m_pInstanceBuffer, 0);
   }

   void Foliage::Render(ID3D11DeviceContext *pD3D11DeviceContext, const d3d::MatrixBuffer &matrix)
   {
	   //Update the the mvp matrix
	   cbMatrix.model = matrix.model;
	   cbMatrix.view  = matrix.view;
	   cbMatrix.proj  = matrix.proj;
	   pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0);
	   pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &m_pMVPBuffer);

	   pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pTextureSRV);
	   pD3D11DeviceContext->PSSetSamplers(0, 1, &m_pTexSamplerState);

	   // Set vertex buffer stride and offset
	   unsigned int strides[2];
	   unsigned int offsets[2];
	   strides[0] = sizeof(Vertex);
	   offsets[0] = 0;
	   
	   strides[1] = sizeof(InstanceType);
	   offsets[1] = 0;

	   ID3D11Buffer *pBuffer[2] = {m_pFoliageVB, m_pInstanceBuffer};

	   pD3D11DeviceContext->IASetVertexBuffers(0, 2, pBuffer, strides, offsets);
	   pD3D11DeviceContext->IASetIndexBuffer(m_pFoliageIB, DXGI_FORMAT_R32_UINT, 0);

	   FoliageShader.use(pD3D11DeviceContext);
	   pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
   }

   void Foliage::Shutdown()
   {
   }

   void Foliage::gen_pos()
   {
	   srand((int)time(NULL));
	   for (int i = 0; i != m_FoliageCount; ++i)
	   {
		   m_FoliageType[i].x = ((float)rand() / (float)(RAND_MAX)) * 9.0f - 4.5f;
		   m_FoliageType[i].z = ((float)rand() / (float)(RAND_MAX)) * 9.0f - 4.5f;

		   auto red = ((float)rand() / (float)(RAND_MAX)) * 1.0f;
		   auto green = ((float)rand() / (float)(RAND_MAX)) * 1.0f;

		   m_FoliageType[i].r = red + 1.0f;
		   m_FoliageType[i].g = green + 0.5f;
		   m_FoliageType[i].b = 0.0f;
	   }
   }

   void Foliage::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
   {
	   HRESULT hr;
	   /////////////////////////////Vertex Buffer//////////////////////////////

	   m_VertexCount = 6;
	   std::vector<Vertex> VertexData(6);
	   //Bottom left
	   VertexData[0].Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	   VertexData[0].Tex = XMFLOAT2(0.0f, 1.0f);

	   VertexData[1].Pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
	   VertexData[1].Tex = XMFLOAT2(0.0f, 0.0f);

	   VertexData[2].Pos = XMFLOAT3(1.0f, 0.0f, 0.0f);
	   VertexData[2].Tex = XMFLOAT2(1.0f, 1.0f);

	   VertexData[3].Pos = XMFLOAT3(1.0f, 0.0f, 0.0f);
	   VertexData[3].Tex = XMFLOAT2(1.0f, 1.0f);

	   VertexData[4].Pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
	   VertexData[4].Tex = XMFLOAT2(0.0f, 0.0f);

	   VertexData[5].Pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	   VertexData[5].Tex = XMFLOAT2(1.0f, 0.0f);


	   D3D11_BUFFER_DESC gridVBDesc;
	   gridVBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	   gridVBDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	   gridVBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	   gridVBDesc.CPUAccessFlags      = 0;
	   gridVBDesc.MiscFlags           = 0;
	   gridVBDesc.StructureByteStride = 0;

	   D3D11_SUBRESOURCE_DATA gridVBO;
	   gridVBO.pSysMem = &VertexData[0];
	   hr = pD3D11Device->CreateBuffer(&gridVBDesc, &gridVBO, &m_pFoliageVB);
	   DebugHR(hr);

	   /////////////////////////////Index Buffer//////////////////////////////

	   m_IndexCount = 6;
	   std::vector<UINT> IndexData(6);
	   for (int i = 0; i != m_IndexCount; ++i)
	   {
		   IndexData[i] = i;
	   }

	   D3D11_BUFFER_DESC gridIBDesc;
	   gridIBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	   gridIBDesc.ByteWidth           = sizeof(UINT) * m_IndexCount;
	   gridIBDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	   gridIBDesc.CPUAccessFlags      = 0;
	   gridIBDesc.MiscFlags           = 0;
	   gridIBDesc.StructureByteStride = 0;

	   D3D11_SUBRESOURCE_DATA girdIBO;
	   girdIBO.pSysMem = &IndexData[0];
	   hr = pD3D11Device->CreateBuffer(&gridIBDesc, &girdIBO, &m_pFoliageIB);
	   DebugHR(hr);

	   ////////////////////////////////Const Buffer//////////////////////////////////////

	   D3D11_BUFFER_DESC mvpDesc;
	   ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	   mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	   mvpDesc.ByteWidth      = sizeof(d3d::MatrixBuffer);
	   mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	   mvpDesc.CPUAccessFlags = 0;
	   mvpDesc.MiscFlags      = 0;
	   hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	   DebugHR(hr);



	   D3D11_BUFFER_DESC instanceBufferDesc;
	   instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	   instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	   instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	   instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	   instanceBufferDesc.MiscFlags = 0;
	   instanceBufferDesc.StructureByteStride = 0;
	   D3D11_SUBRESOURCE_DATA girdIBO;
	   // Setup an initial matrix.
	   D3DXMatrixIdentity(&matrix);

	   // Load the instance array with data.
	   for (i=0; i < m_instanceCount; i++)
	   {
		   m_Instances[i].matrix = matrix;
		   m_Instances[i].color = D3DXVECTOR3(m_foliageArray[i].r, m_foliageArray[i].g, m_foliageArray[i].b);
	   }


	   // Give the subresource structure a pointer to the instance data.
	   instanceData.pSysMem = m_Instances;
	   instanceData.SysMemPitch = 0;
	   instanceData.SysMemSlicePitch = 0;

	   // Create the instance buffer.
	   result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	   if (FAILED(result))
	   {
		   return false;
	   }

   }

   void Foliage::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
   {
	   //Shader interface information
	   std::vector<D3D11_INPUT_ELEMENT_DESC> vInputLayoutDesc;
	   D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc;

	   pInputLayoutDesc.SemanticName         = "POSITION";
	   pInputLayoutDesc.SemanticIndex        = 0;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = 0;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "TEXCOORD";
	   pInputLayoutDesc.SemanticIndex        = 0;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "WORLD";
	   pInputLayoutDesc.SemanticIndex        = 0;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = 0;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "WORLD";
	   pInputLayoutDesc.SemanticIndex        = 1;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "WORLD";
	   pInputLayoutDesc.SemanticIndex        = 2;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "WORLD";
	   pInputLayoutDesc.SemanticIndex        = 3;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   pInputLayoutDesc.SemanticName         = "TEXCOORD";
	   pInputLayoutDesc.SemanticIndex        = 1;
	   pInputLayoutDesc.Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	   pInputLayoutDesc.InputSlot            = 0;
	   pInputLayoutDesc.AlignedByteOffset    =  D3D11_APPEND_ALIGNED_ELEMENT;;
	   pInputLayoutDesc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
	   pInputLayoutDesc.InstanceDataStepRate = 0;
	   vInputLayoutDesc.push_back(pInputLayoutDesc);

	   FoliageShader.init(pD3D11Device, vInputLayoutDesc);
	   FoliageShader.attachVS(L"foliage.vsh", "FoliageVS", "vs_5_0");
	   FoliageShader.attachPS(L"foliage.psh", "FoliagePS", "ps_5_0");
	   FoliageShader.end();
   }

   void Foliage::init_texture(ID3D11Device *pD3D11Device)
   {
	   HRESULT hr;
	   hr = CreateDDSTextureFromFile(pD3D11Device, L"../../media/textures//grass.dds", NULL, &m_pTextureSRV, NULL);
	   DebugHR(hr);

	   // Create a texture sampler state description.
	   D3D11_SAMPLER_DESC samplerDesc;
	   samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	   samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	   samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	   samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	   samplerDesc.MipLODBias = 0.0f;
	   samplerDesc.MaxAnisotropy = 1;
	   samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	   samplerDesc.BorderColor[0] = 0;
	   samplerDesc.BorderColor[1] = 0;
	   samplerDesc.BorderColor[2] = 0;
	   samplerDesc.BorderColor[3] = 0;
	   samplerDesc.MinLOD = 0;
	   samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	   // Create the texture sampler state.
	   hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	   DebugHR(hr);
   }
}