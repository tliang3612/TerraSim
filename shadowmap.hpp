#pragma once
#include "data_factory.h"

struct Shadowmap {
	GLuint textureID;
	GLuint fboID;
	GLuint depthTextureID;
	int shadowMapResolution;

	Shadowmap() = default;

	Shadowmap(int resolution, DataFactory dataFactory)
	{
		this->shadowMapResolution = resolution;

		textureID = dataFactory.CreateTexture();
		fboID = dataFactory.CreateFBO();
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);                                   
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); //create a texture with only depth info

		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void BindFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, shadowMapResolution, shadowMapResolution);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void UnbindFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};