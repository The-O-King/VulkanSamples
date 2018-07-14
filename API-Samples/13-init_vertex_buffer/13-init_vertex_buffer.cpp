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



struct mvpMatrix{
  float posX, posY, posZ, posW,
    posA, posB, posC, posD,
    posE, posF, posG, posH,
    posI, posJ, posK, posL;
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
    attribDescriptions[0].offset = offsetof(Vertex, posX);

    attribDescriptions[1].binding = 0;
    attribDescriptions[1].location = 1;
    attribDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescriptions[1].offset = offsetof(Vertex, colorR);

    return attribDescriptions;
  }
};

const char *vertShaderText =
    "#version 450\n"
    "layout(binding = 0) uniform ubo {\n"
    "  mat4 uMVPMatrix;\n"
    "};\n"
    "layout(location = 0) in vec3 inPosition;\n"
    "layout(location = 1) in vec3 inColor;\n"
    "layout(location = 0) out vec3 fragColor;\n"
    "void main() {\n"
    "  gl_Position = uMVPMatrix * vec4(inPosition, 1.0);\n"
    "  fragColor = inColor;\n"
    "}";

const char *fragShaderText =
    "#version 450\n"
    "layout(location = 0) out vec4 outColor;\n"
    "layout(location = 0) in vec3 fragColor;\n"
    "void main() {\n"
    "  outColor = vec4(fragColor, 1.0);\n"
    "}";


const std::vector<Vertex> verticesIndexed =
{
  {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
  {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
  {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
  {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f}
};

const std::vector<uint16_t> indices = {
  0, 1, 2, 2, 3, 0
};

const std::vector<Vertex> vertices =
{
  {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
  {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
  {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
  //{-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f}
};


void createVertexBuffer(sample_info &info){
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;

  VkBufferCreateInfo buf_info = {};
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.pNext = NULL;
  buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buf_info.size = sizeof(vertices);
  buf_info.queueFamilyIndexCount = 0;
  buf_info.pQueueFamilyIndices = NULL;
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buf_info.flags = 0;
  res = vkCreateBuffer(info.device, &buf_info, NULL, &info.vertex_buffer.buf);
  assert(res == VK_SUCCESS);

  VkMemoryRequirements mem_reqs;
  vkGetBufferMemoryRequirements(info.device, info.vertex_buffer.buf, &mem_reqs);
  VkMemoryAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = NULL;
  alloc_info.memoryTypeIndex = 0;
  alloc_info.allocationSize = mem_reqs.size;

  pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &alloc_info.memoryTypeIndex);

  assert(pass && "No mappable, coherent memory");
  res = vkAllocateMemory(info.device, &alloc_info, NULL, &(info.vertex_buffer.mem));
  assert(res == VK_SUCCESS);

  info.vertex_buffer.buffer_info.range = mem_reqs.size;
  info.vertex_buffer.buffer_info.offset = 0;

  uint8_t *pData;
  res = vkMapMemory(info.device, info.vertex_buffer.mem, 0, mem_reqs.size, 0, (void **)&pData);
  assert(res == VK_SUCCESS);

  memcpy(pData, vertices.data(), sizeof(vertices));
  vkUnmapMemory(info.device, info.vertex_buffer.mem);
  res = vkBindBufferMemory(info.device, info.vertex_buffer.buf, info.vertex_buffer.mem, 0);
  assert(res == VK_SUCCESS);

  info.vi_binding = Vertex::getBindingDescription();
  info.vi_attribs[0] = Vertex::getAttributeDescriptions()[0];
  info.vi_attribs[1] = Vertex::getAttributeDescriptions()[1];
}

void createUniformBuffer(sample_info &info){
  struct mvpMatrix uniformData = {};
  uniformData.posX = 1.0f;
  uniformData.posY = 1.0f;
  uniformData.posZ = 1.0f;
  uniformData.posW = 1.0f;

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

void createFramebuffer(sample_info &info){
  VkResult U_ASSERT_ONLY res;
  bool U_ASSERT_ONLY pass;

  float width = 512;
  float height = 512;

  VkImageCreateInfo imageCreateInfo;
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = NULL;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageCreateInfo.extent.width = width;
  imageCreateInfo.extent.height = height;
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageCreateInfo.flags = 0;

  VkMemoryAllocateInfo allocInfo = {};
  VkMemoryRequirements memReqs;

  info.buffers.resize(1);
  info.swapchainImageCount = 1;
  res = vkCreateImage(info.device, &imageCreateInfo, NULL, &(info.buffers[0].image));
  assert(res == VK_SUCCESS);

  vkGetImageMemoryRequirements(info.device, info.buffers[0].image, &memReqs);
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.pNext = NULL;
  allocInfo.allocationSize = memReqs.size;
  pass = memory_type_from_properties(info, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocInfo.memoryTypeIndex);
  assert(pass && "No device local memory");
  vkAllocateMemory(info.device, &allocInfo, NULL, &(info.buffers[0].mem));
  vkBindImageMemory(info.device, info.buffers[0].image, info.buffers[0].mem, 0);

  VkImageViewCreateInfo imageViewCreateInfo = {};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = NULL;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageViewCreateInfo.subresourceRange = {};
  imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = 1;
  imageViewCreateInfo.image = info.buffers[0].image;
  res = vkCreateImageView(info.device, &imageViewCreateInfo, NULL, &(info.buffers[0].view));
  assert(res == VK_SUCCESS);

  info.framebuffers = (VkFramebuffer *)malloc(info.swapchainImageCount * sizeof(VkFramebuffer));
  VkImageView attachments = info.buffers[0].view;
  VkFramebufferCreateInfo framebufferCreateInfo = {};
  framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferCreateInfo.pNext = NULL;
  framebufferCreateInfo.renderPass = info.render_pass;
  framebufferCreateInfo.attachmentCount = 1;
  framebufferCreateInfo.pAttachments = &attachments;
  framebufferCreateInfo.width = width;
  framebufferCreateInfo.height = height;
  framebufferCreateInfo.layers = 1;
  res = vkCreateFramebuffer(info.device, &framebufferCreateInfo, NULL, info.framebuffers);
  assert(res == VK_SUCCESS);
}

void createDescriptorAndPipelineLayout(sample_info &info){
  VkResult U_ASSERT_ONLY res;

  VkDescriptorSetLayoutBinding layout_bindings[1];
  layout_bindings[0].binding = 0;
  layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_bindings[0].descriptorCount = 1;
  layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layout_bindings[0].pImmutableSamplers = NULL;

  VkDescriptorSetLayoutCreateInfo set0_info = {};
  set0_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  set0_info.pNext = NULL;
  set0_info.flags = 0;
  set0_info.bindingCount = 1;
  set0_info.pBindings = &layout_bindings[0];

  VkDescriptorSetLayout myDescriptorSetLayout[1];
  res = vkCreateDescriptorSetLayout(info.device, &set0_info, NULL, &myDescriptorSetLayout[0]);
  assert(res == VK_SUCCESS);
  info.desc_layout.push_back(myDescriptorSetLayout[0]);

  VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
  pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pPipelineLayoutCreateInfo.pNext = NULL;
  pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
  pPipelineLayoutCreateInfo.setLayoutCount = 1;
  pPipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

  res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo, NULL,
                             &info.pipeline_layout);
  assert(res == VK_SUCCESS);

}

void createDescriptorPoolAndSet(sample_info &info){
  VkResult U_ASSERT_ONLY res;

  VkDescriptorPoolSize type_count[1];
  type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  type_count[0].descriptorCount = 1;


  VkDescriptorPoolCreateInfo poolCreateInfo = {};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCreateInfo.pNext = NULL;
  poolCreateInfo.maxSets = 1;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.pPoolSizes = type_count;

  res = vkCreateDescriptorPool(info.device, &poolCreateInfo, NULL, &info.desc_pool);

  VkDescriptorSetAllocateInfo alloc_info[1];
  alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info[0].pNext = NULL;
  alloc_info[0].descriptorPool = info.desc_pool;
  alloc_info[0].descriptorSetCount = 1;
  alloc_info[0].pSetLayouts = info.desc_layout.data();

  info.desc_set.resize(1);
  res = vkAllocateDescriptorSets(info.device, alloc_info, info.desc_set.data());
  assert (res == VK_SUCCESS);

  VkWriteDescriptorSet writes[1];
  writes[0] = {};
  writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes[0].pNext = NULL;
  writes[0].dstSet = info.desc_set[0];
  writes[0].descriptorCount = 1;
  writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes[0].pBufferInfo = &info.uniform_data.buffer_info;
  writes[0].dstArrayElement = 0;
  writes[0].dstBinding = 0;

  vkUpdateDescriptorSets(info.device, 1, writes, 0, NULL);
}

void my_destroy_framebuffers(sample_info &info){
  for(int x = 0; x < info.swapchainImageCount; x++){
    vkDestroyFramebuffer(info.device, info.framebuffers[x], NULL);
    vkDestroyImage(info.device, info.buffers[x].image, NULL);
    vkDestroyImageView(info.device, info.buffers[x].view, NULL);
    vkFreeMemory(info.device, info.buffers[x].mem, NULL);
  }
}

int sample_main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
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
    info.format = VK_FORMAT_R8G8B8A8_UNORM; //Needed since we do not call init_window which sets the renderpass format
    init_renderpass(info, depthPresent);
    // Custom code for benchmark
    createVertexBuffer(info);
    createUniformBuffer(info);
    createFramebuffer(info);
    createDescriptorAndPipelineLayout(info);
    createDescriptorPoolAndSet(info);
    // Custom code end for benchmark
    init_shaders(info, vertShaderText, fragShaderText);
    init_pipeline_cache(info);
    init_pipeline(info, false, true);
    // Begin renderpass

    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;

    VkRenderPassBeginInfo rp_begin;
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = NULL;
    rp_begin.renderPass = info.render_pass;
    rp_begin.framebuffer = info.framebuffers[0];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = 512;
    rp_begin.renderArea.extent.height = 512;
    rp_begin.clearValueCount = 1;
    rp_begin.pClearValues = clear_values;

    vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
    vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            info.pipeline_layout, 0, 1, info.desc_set.data(), 0, NULL);
    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(info.cmd, 0, 1, &info.vertex_buffer.buf, offsets);

#ifdef __ANDROID__
    // Some drivers don't like setting viewport and scissor
#else
    info.viewport.height = 512.0f;
    info.viewport.width = 512.0f;
    info.viewport.minDepth = 0.0f;
    info.viewport.maxDepth = 1.0f;
    info.viewport.x = 0;
    info.viewport.y = 0;
    vkCmdSetViewport(info.cmd, 0, 1, &info.viewport);

    info.scissor.extent.width = info.width;
    info.scissor.extent.height = info.height;
    info.scissor.offset.x = 0;
    info.scissor.offset.y = 0;
    vkCmdSetScissor(info.cmd, 0, NUM_SCISSORS, &info.scissor);
#endif

    vkCmdDraw(info.cmd, 3, 1, 0, 0);
    vkCmdEndRenderPass(info.cmd);
    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);

    const VkCommandBuffer cmd_bufs[] = {info.cmd};
    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 0;
    submit_info[0].pWaitSemaphores = NULL;
    submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;

    res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
    assert(res == VK_SUCCESS);

        /* Make sure command buffer is finished before presenting */
    do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    // End Render Pass


    // Clean up time
    vkDestroyFence(info.device, drawFence, NULL);
    destroy_pipeline(info);
    destroy_pipeline_cache(info);
    destroy_descriptor_pool(info);
    my_destroy_framebuffers(info); //Using the swapchain container for offscreen framebuffer
    destroy_uniform_buffer(info);
    destroy_vertex_buffer(info);
    destroy_descriptor_and_pipeline_layouts(info);
    destroy_shaders(info);
    destroy_renderpass(info);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_instance(info);
    return 0;
}
