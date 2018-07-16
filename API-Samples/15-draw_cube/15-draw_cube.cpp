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
Draw Cube
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include "cube_data.h"

/* For this sample, we'll start with GLSL so the shader function is plain */
/* and then use the glslang GLSLtoSPV utility to convert it to SPIR-V for */
/* the driver.  We do this for clarity rather than using pre-compiled     */
/* SPIR-V                                                                 */

static const char *vertShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec4 inColor;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = inColor;\n"
    "   gl_Position = myBufferVals.mvp * pos;\n"
    "}\n";

static const char *fragShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (location = 0) in vec4 color;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = color;\n"
    "}\n";

void init_command_buffer2(struct sample_info &info) {
    /* DEPENDS on init_swapchain_extension() and init_command_pool() */
    VkResult U_ASSERT_ONLY res;

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = NULL;
    cmd.commandPool = info.cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    cmd.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd2);
    assert(res == VK_SUCCESS);
}

void destroy_command_buffer2(struct sample_info &info){
    VkCommandBuffer cmd_bufs[1] = {info.cmd2};
    vkFreeCommandBuffers(info.device, info.cmd_pool, 1, cmd_bufs);
}

void init_viewports2(struct sample_info &info) {
#ifdef __ANDROID__
    // Disable dynamic viewport on Android. Some drive has an issue with the dynamic viewport
    // feature.
#else
    info.viewport.height = (float)info.height;
    info.viewport.width = (float)info.width;
    info.viewport.minDepth = (float)0.0f;
    info.viewport.maxDepth = (float)1.0f;
    info.viewport.x = 0;
    info.viewport.y = 0;
    vkCmdSetViewport(info.cmd2, 0, NUM_VIEWPORTS, &info.viewport);
#endif
}

void init_scissors2(struct sample_info &info) {
#ifdef __ANDROID__
    // Disable dynamic viewport on Android. Some drive has an issue with the dynamic scissors
    // feature.
#else
    info.scissor.extent.width = info.width;
    info.scissor.extent.height = info.height;
    info.scissor.offset.x = 0;
    info.scissor.offset.y = 0;
    vkCmdSetScissor(info.cmd2, 0, NUM_SCISSORS, &info.scissor);
#endif
}



int sample_main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Draw Cube";
    const bool depthPresent = true;

    process_command_line_args(info, argc, argv);
    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_window_size(info, 500, 500);
    init_connection(info);
    init_window(info);
    init_swapchain_extension(info);
    init_device(info);

    init_command_pool(info);
    init_command_buffer(info);
    init_command_buffer2(info);
    init_device_queue(info);
    init_swap_chain(info);
    init_depth_buffer(info);
    init_uniform_buffer(info);
    init_descriptor_and_pipeline_layouts(info, false);
    init_renderpass(info, depthPresent);
    init_shaders(info, vertShaderText, fragShaderText);
    init_framebuffers(info, depthPresent);
    init_vertex_buffer(info, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data),
                       sizeof(g_vb_solid_face_colors_Data[0]), false);
    init_descriptor_pool(info, false);
    init_descriptor_set(info, false);
    init_pipeline_cache(info);
    init_pipeline(info, depthPresent);

    /* VULKAN_KEY_START */

    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = 0.2f;
    clear_values[0].color.float32[1] = 0.2f;
    clear_values[0].color.float32[2] = 0.2f;
    clear_values[0].color.float32[3] = 0.2f;
    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = NULL;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    res = vkCreateSemaphore(info.device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
    assert(res == VK_SUCCESS);

    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    res = vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);
    assert(res == VK_SUCCESS);

    for (int x = 0; x < 10000; x++){
      info.current_buffer = x % info.swapchainImageCount;

      // Get the index of the next available swapchain image:
      res = vkAcquireNextImageKHR(info.device, info.swap_chain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
                                  &info.current_buffer);
      // TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
      // return codes
      assert(res == VK_SUCCESS);

      VkRenderPassBeginInfo rp_begin;
      rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      rp_begin.pNext = NULL;
      rp_begin.renderPass = info.render_pass;
      rp_begin.framebuffer = info.framebuffers[info.current_buffer];
      rp_begin.renderArea.offset.x = 0;
      rp_begin.renderArea.offset.y = 0;
      rp_begin.renderArea.extent.width = info.width;
      rp_begin.renderArea.extent.height = info.height;
      rp_begin.clearValueCount = 2;
      rp_begin.pClearValues = clear_values;



      // Record Secondary Command Buffer
      VkCommandBufferInheritanceInfo inherit_info = {};
      inherit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
      inherit_info.pNext = NULL;
      inherit_info.renderPass = info.render_pass;
      inherit_info.subpass = 0;
      inherit_info.framebuffer = info.framebuffers[info.current_buffer];
      inherit_info.occlusionQueryEnable = false;
      inherit_info.queryFlags = 0;
      inherit_info.pipelineStatistics = 0;

      VkCommandBufferBeginInfo cmd_buf_info = {};
      cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      cmd_buf_info.pNext = NULL;
      cmd_buf_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
      cmd_buf_info.pInheritanceInfo = &inherit_info;
      res = vkBeginCommandBuffer(info.cmd2, &cmd_buf_info);
      assert(res == VK_SUCCESS);

      vkCmdBindPipeline(info.cmd2, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
      vkCmdBindDescriptorSets(info.cmd2, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
                              info.desc_set.data(), 0, NULL);

      const VkDeviceSize offsets[1] = {0};
      vkCmdBindVertexBuffers(info.cmd2, 0, 1, &info.vertex_buffer.buf, offsets);

      init_viewports2(info);
      init_scissors2(info);

      vkCmdDraw(info.cmd2, 12 * 3, 1, 0, 0);
      res = vkEndCommandBuffer(info.cmd2);
      assert(res == VK_SUCCESS);
      // Record Secondary Command Buffer End


      // Record Primary Command Buffer Begin
      execute_begin_command_buffer(info);
      vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
      vkCmdExecuteCommands(info.cmd, 1, &info.cmd2);
      vkCmdEndRenderPass(info.cmd);
      vkEndCommandBuffer(info.cmd);
      // Record Primary Command Buffer End


      const VkCommandBuffer cmd_bufs[] = {info.cmd};
      VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      VkSubmitInfo submit_info[1] = {};
      submit_info[0].pNext = NULL;
      submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submit_info[0].waitSemaphoreCount = 1;
      submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
      submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
      submit_info[0].commandBufferCount = 1;
      submit_info[0].pCommandBuffers = cmd_bufs;
      submit_info[0].signalSemaphoreCount = 0;
      submit_info[0].pSignalSemaphores = NULL;

      /* Queue the command buffer for execution */
      res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
      assert(res == VK_SUCCESS);

      /* Now present the image in the window */

      VkPresentInfoKHR present;
      present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      present.pNext = NULL;
      present.swapchainCount = 1;
      present.pSwapchains = &info.swap_chain;
      present.pImageIndices = &info.current_buffer;
      present.pWaitSemaphores = NULL;
      present.waitSemaphoreCount = 0;
      present.pResults = NULL;

      /* Make sure command buffer is finished before presenting */
      do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
      } while (res == VK_TIMEOUT);
      vkResetFences(info.device, 1, &drawFence);

      assert(res == VK_SUCCESS);
      res = vkQueuePresentKHR(info.present_queue, &present);
      assert(res == VK_SUCCESS);
    }

    /* VULKAN_KEY_END */
    if (info.save_images) write_ppm(info, "15-draw_cube");

    vkDestroySemaphore(info.device, imageAcquiredSemaphore, NULL);
    vkDestroyFence(info.device, drawFence, NULL);
    destroy_pipeline(info);
    destroy_pipeline_cache(info);
    destroy_descriptor_pool(info);
    destroy_vertex_buffer(info);
    destroy_framebuffers(info);
    destroy_shaders(info);
    destroy_renderpass(info);
    destroy_descriptor_and_pipeline_layouts(info);
    destroy_uniform_buffer(info);
    destroy_depth_buffer(info);
    destroy_swap_chain(info);
    destroy_command_buffer(info);
    destroy_command_buffer2(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);
    return 0;
}
