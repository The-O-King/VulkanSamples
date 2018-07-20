#include <util_init.hpp>

void primaryCommandBufferBenchmark(sample_info &info, VkClearValue *clear_values, VkFence drawFence, VkSemaphore imageAcquiredSemaphore)
{
  VkResult U_ASSERT_ONLY res;

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

  execute_begin_command_buffer(info);
  vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
  vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
                          info.desc_set.data(), 0, NULL);

  const VkDeviceSize offsets[1] = {0};
  vkCmdBindVertexBuffers(info.cmd, 0, 1, &info.vertex_buffer.buf, offsets);

  init_viewports(info);
  init_scissors(info);

  vkCmdDraw(info.cmd, 0, 1, 0, 0);
  vkCmdEndRenderPass(info.cmd);
  res = vkEndCommandBuffer(info.cmd);
  assert(res == VK_SUCCESS);

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

  // Queue the command buffer for execution
  res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
  assert(res == VK_SUCCESS);

  // Now present the image in the window

  VkPresentInfoKHR present;
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.pNext = NULL;
  present.swapchainCount = 1;
  present.pSwapchains = &info.swap_chain;
  present.pImageIndices = &info.current_buffer;
  present.pWaitSemaphores = NULL;
  present.waitSemaphoreCount = 0;
  present.pResults = NULL;

  // Make sure command buffer is finished before presenting
  do {
    res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
  } while (res == VK_TIMEOUT);
  vkResetFences(info.device, 1, &drawFence);

  assert(res == VK_SUCCESS);
  res = vkQueuePresentKHR(info.present_queue, &present);
  assert(res == VK_SUCCESS);
}

void secondaryCommandBufferBenchmark(sample_info &info, VkClearValue *clear_values, VkFence drawFence, VkSemaphore imageAcquiredSemaphore)
{
  VkResult U_ASSERT_ONLY res;

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

  vkCmdDraw(info.cmd2, 0, 1, 0, 0);
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

  // Queue the command buffer for execution
  res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
  assert(res == VK_SUCCESS);

  // Now present the image in the window

  VkPresentInfoKHR present;
  present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present.pNext = NULL;
  present.swapchainCount = 1;
  present.pSwapchains = &info.swap_chain;
  present.pImageIndices = &info.current_buffer;
  present.pWaitSemaphores = NULL;
  present.waitSemaphoreCount = 0;
  present.pResults = NULL;

  // Make sure command buffer is finished before presenting
  do {
    res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
  } while (res == VK_TIMEOUT);
  vkResetFences(info.device, 1, &drawFence);

  assert(res == VK_SUCCESS);
  res = vkQueuePresentKHR(info.present_queue, &present);
  assert(res == VK_SUCCESS);
}
