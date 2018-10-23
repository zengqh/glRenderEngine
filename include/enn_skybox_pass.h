//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/08/21
// File: enn_skybox_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_skybox_scene_data.h"
#include "enn_mesh.h"

namespace enn
{

class Program;
class CameraNode;
class Mesh;
class SubMesh;
class TextureUnit;
class SkyboxPass : public RenderPassBase
{
public:
	SkyboxPass(RenderPipe* render_pipe);
	~SkyboxPass();

	void setRenderSceneData(SkyboxSceneData* data)
	{
		if (data)
		{
			sky_scene_data_ = data;

			sky_box_valid_ = true;
		}
	}

	const SkyboxSceneData* getRenderSceneData() const
	{
		return sky_scene_data_;
	}

	SkyboxSceneData* getRenderSceneData()
	{
		return sky_scene_data_;
	}


	virtual void prepare();
	virtual void render(Camera* cam);

protected:
	void renderFace(int i, SubMesh* sub_mesh);
	void applyFaceMtr(int i);

protected:
	Program* getRenderEffect();
	void prepareSkybox();
	void prepareMesh();
	void prepareMtr();

protected:
	String makeMacro(bool is_ps);

protected:
	bool getFileData(const String& file_name, String& data);

protected:
	Program*				render_eff_;
	SkyboxSceneData*		sky_scene_data_;
	MeshPtr					sky_box_mesh_;
	TextureUnit*			sky_tex_[6];

	bool					sky_box_valid_;
	bool					is_prepared_;
};

}