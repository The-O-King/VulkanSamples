/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
Create Vertex Buffer
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>


struct shaderInfo{
  float posX, posY;
};

const char* vertShaderText =
    "#version 450 core\n"
    "struct info {\n"
    "  vec2 texCoord;\n"
    "};\n"
    "layout(binding=0, set=0) uniform block {\n"
    "    info infoParameters;\n"
    "};\n"
    "layout(binding=1, set=1) uniform sampler2D infoParameters_samp[2];\n"
    "layout(location=0) out vec4 fragColor;\n"
    "vec4 doSample(info params, sampler2D params_samp[2]) {\n"
    "  return texture(params_samp[0], params.texCoord);\n"
    "}\n"
    "void main() {\n"
    "  info tmp = infoParameters;\n"
    "  fragColor = doSample(tmp, infoParameters_samp);\n"
    "}";

/*
struct mvpMatrix{
  float posX, posY, posZ, posW;
};


struct Vertex{
  float posX, posY, posZ;
  float colorR, colorG, colorB;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(){
    std::vector<VkVertexInputAttributeDescription> attribDescriptions (2);
    attribDescriptions[0].binding = 0;
    attribDescriptions[0].location = 0;
    attribDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescriptions[0].offset = offsetof(Vertex, pos);

    attribDescriptions[1].binding = 0;
    attribDescriptions[1].location = 1;
    attribDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescriptions[1].offset = offsetof(Vertex, color);

    return attribDescriptions;
  }
};

const char *vertShaderText =
    "version 450\n"
    "layout(binding = 0) uniform ubo\n"
    "  mat4 uMVPMatrix;\n"
    "};\n"
    "layout(location = 0) in vec3 inPosition;\n"
    "layout(location = 1) in vec3 inColor;\n"
    "layout(location = 0) out vec3 fragColor;\n"
    "void main() {\n"
    "  gl_position = uMVPMatrix * vec4(inPosition, 1.0);\n";
    "  fragColor = inColor;\n"
    "}";

const char *fragShaderText =
    "#version 450\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "layout(location = 0) out vec4 outColor;\n"
    "layout(location = 0) in vec3 fragColor;\n"
    "void main() {\n"
    "  outColor = vec4(fragColor, 1.0);\n"
    "}";


const std::vector<Vertex> vertices = {
  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
  {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
  0, 1, 2, 2, 3, 0
};
*/

void createUniformBuffer(sample_info &info){
  struct shaderInfo uniformData = {};
  uniformData.posX = 1.0f;
  uniformData.posY = 1.0f;

  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;
  VkBufferCreateInfo buf_info = {};
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.pNext = NULL;
  buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  buf_info.size = sizeof(uniformData);
  buf_info.queueFamilyIndexCount = 0;
  buf_info.pQueueFamilyIndices = NULL;
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buf_info.flags = 0;
  res = vkCreateBuffer(info.device, &buf_info, NULL, &info.uniform_data.buf);
  assert(res == VK_SUCCESS);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(info.device, info.uniform_data.buf, &mem_reqs);
  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.memoryTypeIndex = 0;
  alloc_info.allocationSize = mem_reqs.size;

  pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &alloc_info.memoryTypeIndex);


  assert(pass && "No mappable, coherent memory");
  res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.uniform_data.mem));
  assert(res == VK_SUCCESS);

  u_int8_t *pData;
  res = vkMapMemory(info.device, info.uniform_data.mem, 0, mem_reqs.size, 0, (void**)&pData);
  assert(res == VK_SUCCESS);

  memcpy(pData, &uniformData, sizeof(uniformData));

  vkUnmapMemory(info.device, info.uniform_data.mem);

  res = vkBindBufferMemory(info.device, info.uniform_data.buf, info.uniform_data.mem, 0);
  assert(res == VK_SUCCESS);

  info.uniform_data.buffer_info.buffer = info.uniform_data.buf;
  info.uniform_data.buffer_info.offset = 0;
  info.uniform_data.buffer_info.range = sizeof(uniformData);
}

void createDescriptorAndPipelineLayout(sample_info &info){
  VkResult U_ASSERT_ONLY res;

  VkDescriptorSetLayoutBinding layout_bindings[3];
  layout_bindings[0].binding = 0;
  layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_bindings[0].descriptorCount = 1;
  layout_bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  layout_bindings[0].pImmutableSamplers = NULL;

  layout_bindings[1].binding = 0;
  layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layout_bindings[1].descriptorCount = 1;
  layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  layout_bindings[1].pImmutableSamplers = NULL;

  layout_bindings[2].binding = 1;
  layout_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layout_bindings[2].descriptorCount = 2;
  layout_bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  layout_bindings[2].pImmutableSamplers = NULL;

  VkDescriptorSetLayoutCreateInfo set0_info = {};
  set0_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  set0_info.pNext = NULL;
  set0_info.flags = NULL;
  set0_info.bindingCount = 1;
  set0_info.pBindings = &layout_bindings[0];


  VkDescriptorSetLayoutCreateInfo set1_info = {};
  set1_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  set1_info.pNext = NULL;
  set1_info.flags = NULL;
  set1_info.bindingCount = 2;
  set1_info.pBindings = &layout_bindings[1];

  VkDescriptorSetLayout myDescriptorSetLayout[2];
  res = vkCreateDescriptorSetLayout(info.device, &set0_info, NULL, &myDescriptorSetLayout[0]);
  assert(res == VK_SUCCESS);
  res = vkCreateDescriptorSetLayout(info.device, &set1_info, NULL, &myDescriptorSetLayout[1]);
  assert(res == VK_SUCCESS);
  info.desc_layout.push_back(myDescriptorSetLayout[0]);
  info.desc_layout.push_back(myDescriptorSetLayout[1]);

  VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
  pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pPipelineLayoutCreateInfo.pNext = NULL;
  pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
  pPipelineLayoutCreateInfo.setLayoutCount = 2;
  pPipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

  res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo, NULL,
                             &info.pipeline_layout);

}

void createDescriptorSet(sample_info &info){
  VkResult U_ASSERT_ONLY res;

  VkDescriptorPoolSize type_count[2];
  type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  type_count[0].descriptorCount = 1;
  type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  type_count[1].descriptorCount = 3;


  VkDescriptorPoolCreateInfo poolCreateInfo = {};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCreateInfo.pNext = NULL;
  poolCreateInfo.maxSets = 2;
  poolCreateInfo.poolSizeCount = 2;
  poolCreateInfo.pPoolSizes = type_count;

  res = vkCreateDescriptorPool(info.device, &poolCreateInfo, NULL, &info.desc_pool);

  VkDescriptorSetAllocateInfo alloc_info[1];
  alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info[0].pNext = NULL;
  alloc_info[0].descriptorPool = info.desc_pool;
  alloc_info[0].descriptorSetCount = 2;
  alloc_info[0].pSetLayouts = info.desc_layout.data();

  info.desc_set.resize(2);
  res = vkAllocateDescriptorSets(info.device, alloc_info, info.desc_set.data());
  assert (res == VK_SUCCESS);

  VkWriteDescriptorSet writes[2];
  writes[0] = {};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].pNext = NULL;
  writes[0].dstSet = info.desc_set[0];
  writes[0].descriptorCount = 1;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes[0].pBufferInfo = &info.uniform_data.buffer_info;
  writes[0].dstArrayElement = 0;
  writes[0].dstBinding = 0;
  writes[1] = {};
  writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[1].dstSet = info.desc_set[1];
  writes[1].dstBinding = 1;
  writes[1].descriptorCount = 1;
  writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writes[1].pImageInfo = &info.texture_data.image_info;
  writes[1].dstArrayElement = 0;

  vkUpdateDescriptorSets(info.device, 2, writes, 0, NULL);
}

int sample_main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;
    struct sample_info info = {};
    char sample_title[] = "Vertex Buffer Sample";
    const bool depthPresent = false;

    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_device(info);
    init_command_pool(info);
    init_command_buffer(info);
    execute_begin_command_buffer(info);
    init_device_queue(info);
    init_texture(info);

    createUniformBuffer(info);
    createDescriptorAndPipelineLayout(info);
    createDescriptorSet(info);

    init_renderpass(info, depthPresent);

    init_shaders(info, vertShaderText, nullptr);

    //vkDestroySemaphore(info.device, imageAcquiredSemaphore, NULL);
    vkDestroyBuffer(info.device, info.vertex_buffer.buf, NULL);
    vkFreeMemory(info.device, info.vertex_buffer.mem, NULL);
    destroy_renderpass(info);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_instance(info);
    return 0;
}
