/*******************************************************************************
 * Copyright © 2017-2021 Ezviz Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 * 
 * Contributors:
 * liwei (liwei@ezvizlife.com)
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-25     xurongjun    first version 
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <ezlog.h>
#include "ezos_thread.h"
#include "ez_iot_core_def.h"
#include "ez_iot_core.h"

#include "bulb_led_ctrl.h"
#include "bulb_led_drv_pwm.h"
#include "bulb_business.h"
#include "bulb_protocol_tsl_parse.h"

#include "config_implement.h"
#include "bulb_config_implement.h"

#include "cJSON.h"

#ifdef HAL_ESP
#include "esp_heap_caps.h"
#endif

light_param_t g_bulb_param = {0}; //灯泡所有的参数
ez_thread_t bulb_ctrl_thread;
ez_thread_t bulb_scene_thread;
ez_thread_t bulb_plan_thread;
extern led_current_param_t g_led_current_param; //灯泡实时的亮度
int g_brightness_upper = 100;                   //灯泡实际最高亮度值,从配置文件读取
int g_brightness_lower = 0;                     //灯泡实际最低亮度值,从配置文件读取

/*  场景循环过程中能够中断出来*/
int g_b_SceneChanged;
int g_b_manual_change;


#define MAX_SCENE_COLOR_BLOCK 8

int get_brightness_limit()
{
    // todo.
    // initialize the g_brightness_upper and g_brightness_lower

    return 0;
}

//某些灯泡在低亮度时会闪烁，需要做一个转化
int light_brigtness_to_limit(int light_brightness)
{
    int tmp_brightness = light_brightness;

    if (g_brightness_upper != 0 && g_brightness_upper >= g_brightness_lower)
    {
        tmp_brightness = (tmp_brightness / 100.0) * (g_brightness_upper - g_brightness_lower) + g_brightness_lower;
    }

    return tmp_brightness;
}

/**@fn  远程开灯，柔和渐变打开
 * @brief  
 * @param[in]  
 * @param[out] 
 * @return	  
 */
int regular_power_up(void)
{
    int dstBrigtness = 0;
#if 0
    if (true == check_ota_reboot() && false == g_bulb_param.swit)    //ota 升级且原来是关灯状态，其他物理断电都需要开灯，后面有上报状态同步平台
    {
        ezlog_w(TAG_APP, "ota reboot, no need light up!");
        return -1;
    }
#endif
    dstBrigtness = light_brigtness_to_limit(g_bulb_param.brightness);
    
    switch (g_bulb_param.mode)
    {
    case LIGHT_COLOR:
    {    
        led_ctrl_rgb(dstBrigtness, g_bulb_param.rgb, 10, 1);
        break;
    }
    case LIGHT_WHITE:
    {
        led_ctrl_cct(g_bulb_param.cct,dstBrigtness,10,1);
        break;
    }
    case LIGHT_SCENE:
    {
        ezlog_e(TAG_LIGHT, "turn on the scene mode.");
        // todo:
        break;
	}
    default:
        break;
    }
    return 0;
}


/**@fn  远程开灯，柔和渐变打开
 * @brief  
 * @param[in]  
 * @param[out] 
 * @return	  
 */
int turn_on_lamp()
{
    int dstBrigtness = 0;
    led_ctrl_t led_ctrl_cmd = {0};

    if (false == g_bulb_param.swit)
    {
        g_bulb_param.swit = true;
    }
    else
    {
        ezlog_e(TAG_LIGHT, "receive turn on ctrlcmd,but the bulb switch is already turned on.");
        return -1;
    }

    dstBrigtness = light_brigtness_to_limit(g_bulb_param.brightness);
    switch (g_bulb_param.mode)
    {
    case LIGHT_COLOR:
        /*同时调节亮度和颜色到指定的值*/
        //led_ctrl_rgb(g_led_current_RGB_light,light_brigtness_to_limit(light_cfg.rgb.led_lm_percentage),g_led_current_RGB_value,light_cfg.rgb.color_value,LM_CV_ONECE_TIME,50);

        led_ctrl_cmd.iRgbValue = g_bulb_param.rgb;
        led_ctrl_cmd.nbrightness = light_brigtness_to_limit(g_bulb_param.brightness);
        led_ctrl_cmd.nUpDuration = led_ctrl_cmd.nbrightness * 10; //变化时间500ms,颜色调节接口，颜色主变，亮度在颜色完成变化后，变化到位

        led_ctrl_do_async(&led_ctrl_cmd);
        break;
    case LIGHT_WHITE:
    {
        led_ctrl_cmd.nCctValue = g_bulb_param.cct;
        led_ctrl_cmd.iRgbValue = 0;
        if (dstBrigtness > 80)
        {
            led_ctrl_cmd.nbrightness = 80;
            led_ctrl_cmd.nUpDuration = 1200; //变化时间1200ms
            led_ctrl_do_async(&led_ctrl_cmd);

            led_ctrl_cmd.nbrightness = dstBrigtness;
            led_ctrl_cmd.nUpDuration = (dstBrigtness - 80) * 10;
            led_ctrl_do_sync(&led_ctrl_cmd);
        }
        else
        {
            led_ctrl_cmd.nbrightness = dstBrigtness;
            led_ctrl_cmd.nUpDuration = dstBrigtness * 10; //变化时间为亮度的百分比*10ms,e.g %10亮度，100ms完成，50%亮度，500ms 完成
            led_ctrl_do_async(&led_ctrl_cmd);
        }
    }
    break;
    case LIGHT_SCENE:
        ezlog_e(TAG_LIGHT, "turn on the scene mode.");
        // todo:
        break;
    default:
        break;
    }

    g_b_manual_change = true;

    
    return 0;
}

int turn_off_lamp()
{
    int srcBrigtness = g_led_current_param.cct_value;
    led_ctrl_t led_ctrl_cmd = {0};

    /* 系统测试缺陷：

    已设置倒计时任务，通过app首页切换灯泡开关，查看效果  结果：倒计时任务不取消
    现象二：已设置倒计时任务，通过app详情页切换灯泡开关，查看效果  结果：倒计时任务取消
    修改时区，夏令时，开灯，关灯，统一都取消倒计时任务
    */

    if (true == g_bulb_param.swit)
    {
        g_bulb_param.swit = false;
    }
    else
    {
        ezlog_e(TAG_LIGHT, "receive turn off ctrlcmd,but the bulb switch is already turned off.");
        return -1;
    }

    switch (g_bulb_param.mode)
    {
    case LIGHT_COLOR:
    {
        led_ctrl_cmd.iRgbValue = g_led_current_param.rgb;
        led_ctrl_cmd.nbrightness = 0;
        led_ctrl_cmd.nUpDuration = srcBrigtness * 10; //变化时间根据亮度来变化
        led_ctrl_do_async(&led_ctrl_cmd);
    }
    break;
    case LIGHT_WHITE:
    {
        led_ctrl_cmd.nCctValue = g_led_current_param.cct_value;

        led_ctrl_cmd.iRgbValue = 0;
        if (srcBrigtness > 80)
        {

            led_ctrl_cmd.nbrightness = 80;
            led_ctrl_cmd.nUpDuration = (srcBrigtness - 80) * 10; //变化时间100ms
            led_ctrl_do_async(&led_ctrl_cmd);

            led_ctrl_cmd.nbrightness = 0;
            led_ctrl_cmd.nUpDuration = 1200; //变化时间1200ms，就白灯最高亮度熄灭采用特殊的延时时间
            led_ctrl_do_sync(&led_ctrl_cmd);
        }
        else
        {
            led_ctrl_cmd.nbrightness = 0;
            led_ctrl_cmd.nUpDuration = srcBrigtness * 10; //变化时间100ms
            led_ctrl_do_async(&led_ctrl_cmd);
        }
    }
    break;

    case LIGHT_SCENE:
    case LIGHT_MUSIC: //暂时没用上，理论上灯泡在律动情况下，不会收到关灯指令，APP 的一切操作之前都需要关掉律动功能。
    {

        /* 场景合音乐律动的状态不确定，因不知初始状态(白灯或者彩灯)，需要关两次，因为本身为变化的，这里立即关闭灯泡
         	彩灯正常比较暗，先关彩灯，不会太明显（否则彩灯色块下，白灯方式关闭，会用白灯方式将亮度调到0，显示为闪烁），再关白灯时，若此时亮度已经为0，不会在闪烁
			后续灯控都可以采用此方式 进行控制，不需要 区分灯的产品类型，灯的类型可以在底层屏蔽，白光灯关两次，彩光灯关闭底层将不执行
         */
        led_ctrl_cmd.nbrightness = 0;
        led_ctrl_cmd.nUpDuration = 10; //立即关闭
        led_ctrl_cmd.iRgbValue = g_led_current_param.rgb;
        led_ctrl_do_async(&led_ctrl_cmd);

        led_ctrl_cmd.iRgbValue = 0; //rgbvalue 置0 采用白光方式调亮度
        led_ctrl_cmd.nCctValue = g_led_current_param.cct_value;
        led_ctrl_do_sync(&led_ctrl_cmd); //等待上一条指令执行完毕后执行白灯关闭
    }

    break;
    default:
        break;
    }

    g_b_manual_change = true;
    
    return 0;
}

// note 1-100 亮度
int adjust_light_brightness(int light_brightness)
{
    led_ctrl_t led_ctrl_cmd = {0};
    int iChangeRange = 10;

    ezlog_v(TAG_LIGHT, "function %s in.", __func__);

    if (g_bulb_param.brightness == light_brightness) //设置的亮度与设备APP 记录的亮度一致，不变化，正常APP 不会进入
    {
        ezlog_d(TAG_LIGHT, "brightness is not changed.");
        return 0;
    }

    if (g_bulb_param.brightness > light_brightness)
    {
        iChangeRange = g_bulb_param.brightness - light_brightness; //采用实际亮度值进行计算渐变时间
    }
    else
    {
        iChangeRange = light_brightness - g_bulb_param.brightness;
    }
    int tmp_brightness = light_brigtness_to_limit(light_brightness);
    /*调节到目标亮度*/
    led_ctrl_cmd.nbrightness = tmp_brightness;

    led_ctrl_cmd.nUpDuration = iChangeRange * 10; //根据亮度差值来计算变化时间

    if (tmp_brightness > 100 || tmp_brightness < 0)
    {
        ezlog_e(TAG_LIGHT, "light brightness param error.");
        return -1;
    }

    switch (g_bulb_param.mode)
    {
    case LIGHT_COLOR:

        led_ctrl_cmd.iRgbValue = g_bulb_param.rgb;
        if (0 != led_ctrl_do_async(&led_ctrl_cmd))
        {
            ezlog_e(TAG_LIGHT, "color-mode adjust light_color failed.");
            return -1;
        }
        break;
    case LIGHT_WHITE:
    {
        led_ctrl_cmd.nCctValue = g_bulb_param.cct;
        if (0 != led_ctrl_do_async(&led_ctrl_cmd))
        {
            ezlog_e(TAG_LIGHT, "white-mode adjust light_color failed.");
            return -1;
        }
    }
    break;
    default:
        ezlog_e(TAG_LIGHT, "mode =%d,is not support adjust bright.");
        break;
    }

    g_bulb_param.brightness = light_brightness;

    g_b_manual_change = true;

    return 0;
}

int adjust_light_cct(int light_cct)
{

    led_ctrl_t led_ctrl_cmd = {0};
    int iChangeRange = 10;
    ezlog_v(TAG_LIGHT, "function %s in.", __func__);


    if (2700 > light_cct || 6500 < light_cct)
    {
        ezlog_e(TAG_LIGHT, "light cct param error.");
        return -1;
    }
    if (g_bulb_param.cct > light_cct)
    {
        iChangeRange = g_bulb_param.cct - light_cct; //色温变化的时长
    }
    else
    {
        iChangeRange = light_cct - g_bulb_param.cct; //色温变化的时长
    }
    led_ctrl_cmd.nbrightness = g_bulb_param.brightness;
    led_ctrl_cmd.nCctValue = light_cct;
    led_ctrl_cmd.nUpDuration = iChangeRange / 10; //实现渐变效果，最长6500-2700= 380 ms

    if (0 != led_ctrl_do_async(&led_ctrl_cmd))
    {
        ezlog_e(TAG_LIGHT, "white-mode adjust light_color failed.");
        return -1;
    }

    g_bulb_param.cct = light_cct;
    if (LIGHT_WHITE != g_bulb_param.mode)
    {
        /*
            app 正常操作情况下， 
            mode 应该已经为白光模式，不排除调用方式直接从彩光发色温进行调节，跳到白光。
        */
        g_bulb_param.mode = LIGHT_WHITE;

        user_property_report("WorkMode");
    }

    g_b_manual_change = true;
    
    return 0;
}

/*只调节颜色，不调节亮度*/
int adjust_light_rgb(int light_rgb)
{
    int iRet = 0;
    led_ctrl_t led_ctrl_cmd = {0};
    ezlog_v(TAG_LIGHT, "function %s in.", __func__);



    if (light_rgb > 0xffffff || light_rgb < 0)
    {
        ezlog_e(TAG_LIGHT, "light rgb param error.");
        return -1;
    }

    /* 由当前颜色调节到目标颜色，当前亮度与目标亮度一致都是g_led_current_RGB_light*/
    led_ctrl_cmd.iRgbValue = light_rgb;
    led_ctrl_cmd.nbrightness = g_bulb_param.brightness;
    led_ctrl_cmd.nUpDuration = 0; //变化时间由内部算法自己控制

    iRet = led_ctrl_do_async(&led_ctrl_cmd);

    g_bulb_param.rgb = light_rgb;
    if (LIGHT_COLOR != g_bulb_param.mode)
    {
        g_bulb_param.mode = LIGHT_COLOR;
        user_property_report("WorkMode");
    }
    /* 将g_bulb_param 参数保存到flash 中*/
    printf("\n to_do DEBUG in line (%d) and function (%s)): save bulb param to flash\n ", __LINE__, __func__);

    g_b_manual_change = true;

    return iRet;
}

void light_scene_task(void *param)
{
    ezlog_i(TAG_LIGHT, "light scene index is %d.", g_bulb_param.scene.cb_count);
    int i = 0;
    light_scene_t scene = {0};
    led_ctrl_t led_ctrl_cmd = {0};

    int rand[MAX_SCENE_COLOR_BLOCK] = {0, 1, 2, 3, 4, 5, 6, 7}; //一个场景最多8个颜色
    int candleDurTimeRand = 0;
    int candleSpeedRand = 0;

    do
    {
        if ((g_bulb_param.mode != LIGHT_SCENE)
            ||(0 == scene.cb_count)
            )
        {
            ezos_delay_ms(1000);
            continue;
        }
        g_b_SceneChanged = false; //新的场景执行时，退出标志位清空

        memcpy(&scene, &g_bulb_param.scene, sizeof(light_scene_t));

        if (scene.cb_count == 1) //静态场景
        {
            ezlog_v(TAG_LIGHT, "cct:%d, rgb:%x, bright:%d", scene.color_temperature[0],
                    scene.color_rgb[0],
                    scene.brightness[0]);

            led_ctrl_cmd.nbrightness = light_brigtness_to_limit(scene.brightness[0]);
            led_ctrl_cmd.nUpDuration = 20;

            if (scene.type[0] == 0) //白光
            {
                led_ctrl_cmd.nCctValue = scene.color_temperature[0];
                led_ctrl_cmd.iRgbValue = 0;
                led_ctrl_do_sync(&led_ctrl_cmd);
            }
            else if (scene.type[0] == 1) //彩光
            {
                led_ctrl_cmd.iRgbValue = scene.color_rgb[0];
            }
            led_ctrl_do_async(&led_ctrl_cmd);

            ezos_delay_ms(200);
        }
        else //动态场景，有多个色块
        {

            /* 随机变化 场景需要初始化下随机数组，后面一轮颜色变化按照这个随机序列进行*/
            if (1 == scene.random)
            {
#ifdef SCENE_RANDOM
                srand((unsigned int)time(NULL));
                randomlize(rand, scene.cb_count);
#endif
                ezlog_v(TAG_LIGHT, "light number is %d,random sequence is %d,%d,%d,%d,%d,%d,%d,%d", scene.cb_count,
                        rand[0], rand[1], rand[2], rand[3], rand[4], rand[5], rand[6], rand[7]);
            }
            else
            {
                /* 其他场景恢复正序*/
                for (i = 0; i < MAX_SCENE_COLOR_BLOCK; i++)
                {
                    rand[i] = i;
                }
            }

            for (i = 0; i < scene.cb_count; i++)
            {
#ifdef SCENE_RANDOM
                if (0 == scene.random)
                {
                    candleDurTimeRand = scene.duration * (10 + rand[i]) / 10; //每一个变化时间按照1.1, 1.2, 1.3 ....1.8  倍变化
                    candleSpeedRand = scene.speed * (10 + rand[i]) / 10;      //每一个色块持续时间1.1, 1.2, 1.3 ....1.8  倍变化
                }
                else
#endif
                {
                    candleDurTimeRand = scene.duration;
                    candleSpeedRand = scene.speed;
                }
                ezlog_v(TAG_LIGHT, "cb index=%d,cct:%d, rgb:%x, bright:%d,rand[%d]=%d,duration=%d,type=%d",
                        i, scene.color_temperature[rand[i]],
                        scene.color_rgb[rand[i]],
                        scene.brightness[rand[i]], i, rand[i], candleDurTimeRand, scene.type[rand[i]]);

                if (g_b_SceneChanged)
                {
                    /*
					外部场景变化时，for 循环需要及时退出
					若这里不退出，当index = 8时，很容易复现问题,将不会从新场景的第一个颜色开始 liwei@20200819
					*/
                    g_b_SceneChanged = false;
                    break;
                }

                led_ctrl_cmd.nUpDuration = candleDurTimeRand;
                if (scene.type[rand[i]] == 0) //white
                {
                    led_ctrl_cmd.nCctValue = scene.color_temperature[rand[i]];
                    led_ctrl_cmd.nbrightness = light_brigtness_to_limit(scene.brightness[rand[i]]);
                    led_ctrl_cmd.iRgbValue = 0;
                    led_ctrl_do_async(&led_ctrl_cmd);
                }
                else if (scene.type[rand[i]] == 1) //color
                {

                    led_ctrl_cmd.nCctValue = 0;
                    led_ctrl_cmd.nbrightness = light_brigtness_to_limit(scene.brightness[rand[i]]);
                    led_ctrl_cmd.iRgbValue = scene.color_rgb[rand[i]];
                    led_ctrl_do_async(&led_ctrl_cmd);
                }
                ezos_delay_ms(led_ctrl_cmd.nUpDuration); //这里应该用同步方式发送调灯指令
            }
        }
    } while (true);

    return;
}

/**@fn		  
 * @brief		  已经在场景模式下了，调整场景模式，比如阅读模式到浪漫模式，走shadow 
方式传递过来进行调用，
 				  先关闭原来的场景模式，再重启场景模式
 
 * @param[in]  
 * @param[out] 
 * @return	  
 */

int set_light_scene(char *light_scene)
{
    light_scene_t *p_scene = &g_bulb_param.scene;

    json2param_CustomSceneCfg(light_scene,p_scene);
    
    g_b_SceneChanged = true;
    return 0;
}



int disable_light_switch_plan(int index)
{
    ez_int32_t rv = -1;
    ez_int32_t buf_len = 1024 * 2;
    ez_int8_t *buf = (ez_int8_t *)malloc(buf_len);
    cJSON *js_root = NULL;
    ezlog_w(TAG_LIGHT, "disable_switch_plan, index = %d", index);

    do
    {
        if (NULL == buf)
        {
            break;
        }

        rv = config_get_value(K_LIGHTSWITCHPLAN, buf, &buf_len);
        if (0 != rv)
        {
            ezlog_e(TAG_APP, "get light switch plan failed.");
            break;
        }

        js_root = cJSON_Parse((char *)buf);
        if (NULL == js_root || cJSON_Object!= js_root->type)
        {
            ezlog_e(TAG_LIGHT, "parse json failed.");
            break;
        }

        cJSON *js_array = cJSON_GetObjectItem(js_root,"plan");
        if (NULL == js_array || cJSON_Array!= js_array->type)
        {
            ezlog_e(TAG_LIGHT, "parse json failed.");
            break;
        }
        
        if (index >= cJSON_GetArraySize(js_array))
        {
            break;
        }
        cJSON *js_obj = cJSON_GetArrayItem(js_array, index);
        if (NULL == js_obj)
        {
            break;
        }

        cJSON *pJsonEnable = cJSON_GetObjectItem(js_obj, "enabled");
        if (NULL == pJsonEnable)
        {
            break;
        }

        pJsonEnable->valueint = 0;
        pJsonEnable->valuedouble = 0;

        char *buf_new = cJSON_PrintUnformatted(js_root);
        if (NULL == buf_new)
        {
            break;
        }

        if (0 != config_set_value(K_LIGHTSWITCHPLAN, (void *)buf_new, strlen(buf_new)))
        {
            free(buf_new);
            break;
        }

        g_bulb_param.swit_plan.plan[index].enabled = 0;
        free(buf_new);
        rv = 0;
    } while (0);

    buf ? free(buf) : 0;
    js_root ? cJSON_Delete(js_root) : 0;

    user_property_report("LightSwitchPlan");

    if (0 != rv)
    {
        ezlog_e(TAG_LIGHT, "disable_light_switch_plan, rv = %d", rv);
    }

    return rv;
}

int set_light_plan(char *json_string)
{
    switch_plan_t *p_light_plan = &g_bulb_param.swit_plan;

    json2param_plan_conf(json_string,p_light_plan);

    return 0;
}

int disable_light_sleep_plan(int index,ez_bool_t helpsleep)
{
    ez_int32_t rv = -1;
    ez_int32_t buf_len = 1024 * 2;
    ez_int8_t *buf = (ez_int8_t *)malloc(buf_len);
    cJSON *js_root = NULL;
    ezlog_w(TAG_LIGHT, "disable_switch_plan, index = %d", index);

    do
    {
        if (NULL == buf)
        {
            break;
        }
        if(helpsleep)
        {
            rv = config_get_value(K_HELPSLEEP, buf, &buf_len);
        }
        else
        {
            rv = config_get_value(K_WAKEUP, buf, &buf_len);
        }

        if (0 != rv)
        {
            ezlog_e(TAG_APP, "get light switch plan failed.");
            break;
        }

        js_root = cJSON_Parse((char *)buf);
        if (NULL == js_root || cJSON_Array!= js_root->type)
        {
            ezlog_e(TAG_LIGHT, "parse json failed.");
            break;
        }
        
        if (index >= cJSON_GetArraySize(js_root))
        {
            break;
        }
        cJSON *js_obj = cJSON_GetArrayItem(js_root, index);
        if (NULL == js_obj)
        {
            break;
        }

        cJSON *pJsonEnable = cJSON_GetObjectItem(js_obj, "enabled");
        if (NULL == pJsonEnable)
        {
            break;
        }

        pJsonEnable->valueint = 0;
        pJsonEnable->valuedouble = 0;

        char *buf_new = cJSON_PrintUnformatted(js_root);
        if (NULL == buf_new)
        {
            break;
        }

        if(helpsleep)
        {
            config_set_value(K_HELPSLEEP, (void *)buf_new, strlen(buf_new));
            g_bulb_param.help_sleep_plan.plan[index].enabled = 0;
            user_property_report("HelpSleep");

        }
        else
        {
            config_set_value(K_WAKEUP, (void *)buf_new, strlen(buf_new));
            g_bulb_param.wakeup_plan.plan[index].enabled = 0;
            user_property_report("WakeUp");
        }
        free(buf_new);
        rv = 0;
    } while (0);

    buf ? free(buf) : 0;
    js_root ? cJSON_Delete(js_root) : 0;


    if (0 != rv)
    {
        ezlog_e(TAG_LIGHT, "disable_light_switch_plan, rv = %d", rv);
    }

    return rv;
}

int set_light_wakeup(char *json_string)
{

    light_sleep_plan_t *p_wakeup = &g_bulb_param.wakeup_plan;

    json2param_sleep_conf(json_string, p_wakeup);

    return 0;
}

int set_light_helpsleep(char *json_string)
{
    light_sleep_plan_t *p_helpsleep = &g_bulb_param.help_sleep_plan;

    json2param_sleep_conf(json_string, p_helpsleep);

    return 0;
}

int disable_countdown()
{
    ez_int32_t rv = -1;
    ez_int32_t buf_len = 512 ;
    ez_int8_t *buf = (ez_int8_t *)malloc(buf_len);
    cJSON *js_root = NULL;
    
    do
    {
        if (NULL == buf)
        {
            break;
        }

        rv = config_get_value(K_COUNTDOWNCFG, buf, &buf_len);
        if (0 != rv)
        {
            ezlog_e(TAG_APP, "get light switch plan failed.");
            break;
        }

        js_root = cJSON_Parse((char *)buf);
        if (NULL == js_root || cJSON_Object!= js_root->type)
        {
            ezlog_e(TAG_LIGHT, "parse json failed.");
            break;
        }

        cJSON *pJsonEnable = cJSON_GetObjectItem(js_root, "enable");
        if (NULL == pJsonEnable)
        {
            break;
        }

        pJsonEnable->valueint = 0;
        pJsonEnable->valuedouble = 0;

        char *buf_new = cJSON_PrintUnformatted(js_root);
        if (NULL == buf_new)
        {
            break;
        }

        if (0 != config_set_value(K_LIGHTSWITCHPLAN, (void *)buf_new, strlen(buf_new)))
        {
            free(buf_new);
            break;
        }

        g_bulb_param.countdown.enabled = 0;
        free(buf_new);
        rv = 0;
    } while (0);

    buf ? free(buf) : 0;
    js_root ? cJSON_Delete(js_root) : 0;

    user_property_report("CountdownCfg");

    if (0 != rv)
    {
        ezlog_e(TAG_LIGHT, "disable_countdown cfg, rv = %d", rv);
    }

    return rv;
}

void *action_get_countdown()
{

    cJSON *js_root = NULL;
    char *buf_json_format = NULL;
    do
    { 
        js_root = cJSON_CreateObject();
        if (NULL == js_root)
        {          
            ezlog_e(TAG_APP, "creat countdown json failed,no memory");
        }

        if(g_bulb_param.countdown.enabled)
        {
            cJSON_AddTrueToObject(js_root, "enable");
        }
        else
        {
            cJSON_AddFalseToObject(js_root, "enable");
        }

        if(g_bulb_param.countdown.swit)
        {
            cJSON_AddTrueToObject(js_root, "switch");
        }
        else
        {
            cJSON_AddFalseToObject(js_root, "switch");
        }
       
        cJSON_AddNumberToObject(js_root, "timeRemaining", g_bulb_param.countdown.time_remain_now);
   
        ezos_time_t time_now = ezos_time(NULL);
        cJSON_AddNumberToObject(js_root, "reportTimestamp", time_now);
 
        buf_json_format = cJSON_PrintUnformatted(js_root);

        if (NULL == buf_json_format)
        {
            ezlog_e(TAG_APP, "format countdown json failed,no memory");
            break;
        }
    } while (0);

    js_root ? cJSON_Delete(js_root) : 0;

    return buf_json_format;
}


int set_light_countdown(char *json_string)
{

    switch_countdown_t *p_countdown = &g_bulb_param.countdown;

    json2param_countdown(json_string, p_countdown);

    return 0;
}





int set_light_biorhythm(char *json_string)
{
    biorhythm_t *p_biorhythm_plan = &g_bulb_param.biorhythm_plan;
    
    json2param_biorhythm(json_string, p_biorhythm_plan);

    return 0;
}

int reset_light(int mode)
{
    ezlog_i(TAG_LIGHT, "function %s in.reset light.mode =%d", __func__, mode);

    if (0 == mode) //完全恢复出厂
    {
        printf("\n to_do DEBUG in line (%d) and function (%s)): \n ", __LINE__, __func__);
    }
    else if (2 == mode) //除开网络，产测参数，服务器以外的信息
    {
        printf("\n to_do DEBUG in line (%d) and function (%s)): \n ", __LINE__, __func__);
        //todo
    }
    else if (3 == mode) //除开产测参数，以外的信息,退货换到其他用户手里
    {
        printf("\n to_do DEBUG in line (%d) and function (%s)): \n ", __LINE__, __func__);
        //todo
    }
    else
    {
        printf("\n to_do DEBUG in line (%d) and function (%s)): \n ", __LINE__, __func__);
        //todo
    }

    //todo

    return 0;
}

/**
 * @brief 周重复开关计划的执行

   单次执行的计划，执行过一次后不再进入此函数判断
 * 
 * @param pplan 单个计划，计划是否需要重复
 */
static ez_bool_t cycle_plan_do_swit(switch_plan_metadata_t *pplan,ez_bool_t week_repeat)
{
    ez_bool_t is_done = false;
    ezos_time_t time_now = ezos_time(NULL);
    struct ezos_tm ptm_time_now = {0};
    if (NULL == ezos_localtime(&time_now, &ptm_time_now))
    {
        ezlog_w(TAG_LIGHT, "ezos_localtime error");
        return -1;
    }

    ez_int8_t week = ptm_time_now.tm_wday;

    do
    {
        if(week_repeat && (0 == pplan->week_days[week]))
        {
            return -1;            
        }

        if ((ptm_time_now.tm_hour == pplan->begin.tm_hour) 
            && (ptm_time_now.tm_min == pplan->begin.tm_min))
        {
            ezlog_w(TAG_LIGHT, "plan begin timeup!!!,pplan->count=%d", pplan->start_count);
            /*
            定时计划精确度是1分钟，若在这一分钟执行了定时计划关灯，但用户又手动开灯，此时定时计划不应该重复执行
            设备设置的定时器是1s 进入一次，所以会多次is_timeup，
            增加一个计数器，只保证执行一次。
            
            */
            if (0 == pplan->start_count)
            {
                if (0 == pplan->action)
                {
                    turn_off_lamp();
                }
                else
                {
                    turn_on_lamp();
                }
                is_done = 1;
            }
            pplan->start_count++;
            if (pplan->start_count > 60) //定时器1s 进入一次，执行超过3*20=60s后，count 清零，用于周重复计划的下一次能够进入
            {
                pplan->start_count = 0;
            }
            break;
        }

        // 如果时间相等，说明计划是时刻计划，不是时间段计划，无需执行停止动作
        if (0 == memcmp(&pplan->begin, &pplan->end, sizeof(pplan->begin)))
        {
            ezlog_v(TAG_LIGHT, "plan end no need to exec!");
            break;
        }
        is_done = 0;

        if ((ptm_time_now.tm_hour == pplan->end.tm_hour) 
            && (ptm_time_now.tm_min == pplan->end.tm_min))
        {
            ezlog_w(TAG_LIGHT, "plan end timeup!!!,pplan->count=%d", pplan->end_count);
            if (0 == pplan->end_count)
            {
                if (0 == pplan->action)
                {
                    turn_on_lamp();
                }
                else
                {
                    turn_off_lamp();
                }
                is_done = 1;
            }
            pplan->end_count++;
            if (pplan->end_count > 60) //定时器1s 进入一次，执行60s后，count 清零，用于周重复计划的下一次能够进入
            {
                pplan->end_count = 0;
            }

            break;
        }
    } while (0);

    return is_done;
}


/**
 * @brief 入睡唤醒计划

   长时间执行的一个计划
 * 
 * @param pplan 单个计划，计划是否需要重复
 */
static ez_bool_t cycle_plan_do_sleep(light_sleep_metadata_t *pplan,ez_bool_t week_repeat,ez_bool_t helpsleep)
{
    ez_bool_t is_done = 0;
    led_ctrl_t led_ctrl_cmd = {0};
    ez_int32_t min_start;
    ez_int32_t min_now;
    ezos_time_t time_now = ezos_time(NULL);
    int brightness_change =0;
    int interval = 0;

    struct ezos_tm ptm_time_now = {0};
    if (NULL == ezos_localtime(&time_now, &ptm_time_now))
    {
        ezlog_w(TAG_LIGHT, "ezos_localtime error");
        return -1;
    }

    ez_int8_t week = ptm_time_now.tm_wday;

    do
    {
        if(week_repeat && (0 == pplan->week_days[week]))
        {
            break;            
        }
        if(helpsleep)
        {
            min_start = pplan->begin.tm_hour*60 + pplan->begin.tm_min;
        }
        else
        {
            min_start = pplan->begin.tm_hour*60 + pplan->begin.tm_min - pplan->fade_time;
        }
    
        min_now = ptm_time_now.tm_hour*60 + ptm_time_now.tm_min;

        //printf("\n LW_PRINT DEBUG in line (%d) and function (%s)): min_start=%d %d,week=%d helpsleep=%d,count =%d\n ",__LINE__, __func__,min_start,min_now,week,helpsleep,pplan->start_count );
        
        
        if (min_now >= min_start
            && min_now < (min_start + pplan->fade_time +1) //有一定的误差，能够多次进入进行亮度调节 ，直到亮度调节完毕
            && 0xffffffff != pplan->start_count 
            )
        {

            ezlog_v(TAG_LIGHT, "sleep process!!!,pplan->count=%d,start_time=%d:%d,fadetime=%d,week=%d"
                , pplan->start_count,pplan->begin.tm_hour,pplan->begin.tm_min,pplan->fade_time,week);
            
            /* 每1s钟 的亮度，间隔一定时间，变化%1，总秒 数分布到 需要变化的亮度范围上 
            e.g 5分钟 从20%亮度 熄灭，则每隔15s 变化1%，0到14s为0%，15到29s 为%1，....275-299s为19,300s 熄灭
            */
            interval =(pplan->fade_time*60)/ pplan->action.brightness;
            
            /*提前结束的情况*/
            if((false == get_light_switch() && true == helpsleep) //入睡计划灯在熄灭的情况不执行入睡,
                ||(true == get_light_switch() && false == helpsleep)   //唤醒计划，灯在开灯的情况下不执行唤醒,
                || (( pplan->start_count > 0) && (true == g_b_manual_change))  //已经开始执行入睡 ，且外部进行了灯控
                || (0 == interval)   //淡出时间参数异常
                )
            {
                pplan->start_count = 0xffffffff;
                g_b_manual_change = false;
                is_done = 1;
                break;
            }

            if(0 == pplan->start_count) //开始进入睡眠循环调整亮度过程，用一个全局变量来表明外部可以中断
            {
                g_b_manual_change = false;
            }
      
            /*按照起始时刻计算 调整的亮度，通过start_count计数调整。*/                
            led_ctrl_cmd.iRgbValue = pplan->action.color_rgb;
            
            brightness_change  = pplan->start_count / interval;
            if(helpsleep)
            {
                led_ctrl_cmd.nbrightness = pplan->action.brightness - brightness_change;
            }
            else
            {
                led_ctrl_cmd.nbrightness = brightness_change + 1 ;
            }
            led_ctrl_cmd.nCctValue = pplan->action.color_temperature;
            led_ctrl_cmd.nUpDuration = 2000; //2s渐变%1 的亮度

            led_ctrl_do_async(&led_ctrl_cmd);
            pplan->start_count ++;
            if(0 == led_ctrl_cmd.nbrightness)
            {
                pplan->start_count = 0xffffffff;
                is_done = 1;
                break;
            }
            if(pplan->start_count > 2 * 60 * 60)  //2小时后，count 清零，用于周重复计划的下一次能够进入
            {
                pplan->start_count = 0;  
            }
        }        
    } while (0);

    return is_done;
}




static void switch_plan_timer()
{
    switch_plan_t *p_plan = &g_bulb_param.swit_plan;
    ez_int8_t week_days_zero[7] = {0};
    int i;
    for (i = 0; i < p_plan->count; i++)
    {
        if (0 == p_plan->plan[i].enabled)
        {
            continue;
        }

        ezlog_v(TAG_LIGHT, "plan[%d], %d %d %d %d %d %d %d", i,
                p_plan->plan[i].week_days[0], p_plan->plan[i].week_days[1],
                p_plan->plan[i].week_days[2], p_plan->plan[i].week_days[3],
                p_plan->plan[i].week_days[4], p_plan->plan[i].week_days[5],
                p_plan->plan[i].week_days[6]);

        if (0 == memcmp(week_days_zero, p_plan->plan[i].week_days, sizeof(week_days_zero)))
        {
            if(true == cycle_plan_do_swit(&p_plan->plan[i], false))
            {
                ezlog_i(TAG_LIGHT,"once plan is done,report status to shadow");
                disable_light_switch_plan(i);
            }
        }
        else
        {
            cycle_plan_do_swit(&p_plan->plan[i],true);
        }
    }
}

void countdown_timer()
{
    switch_countdown_t *p_countdown = &g_bulb_param.countdown;

    if (false == p_countdown->enabled)
    {
        return;
    }
    
    ezos_time_t time_now = ezos_time(NULL);

    p_countdown->time_remain_now = time_now - p_countdown->time_stamp ;
     
    if (p_countdown->time_remain_now <= 0)
    {
        p_countdown->time_remain_now = 0;
        ezlog_w(TAG_LIGHT, "countdown timeup!!!");
        if (0 == p_countdown->swit)
        {
            turn_off_lamp();
        }
        else
        {
            turn_on_lamp();
        }
        
        disable_countdown();
    }

    return;
}

void biorhythm_plan_timer()
{
    

}

void sleep_plan_timer()
{
    light_sleep_plan_t *p_plan = &g_bulb_param.help_sleep_plan;
    ez_int8_t week_days_zero[7] = {0};
    int i;
    for (i = 0; i < p_plan->count; i++)
    {
        if (0 == p_plan->plan[i].enabled)
        {
            continue;
        }

        ezlog_v(TAG_LIGHT, "sleep plan[%d], %d %d %d %d %d %d %d", i,
                p_plan->plan[i].week_days[0], p_plan->plan[i].week_days[1],
                p_plan->plan[i].week_days[2], p_plan->plan[i].week_days[3],
                p_plan->plan[i].week_days[4], p_plan->plan[i].week_days[5],
                p_plan->plan[i].week_days[6]);

        if (0 == memcmp(week_days_zero, p_plan->plan[i].week_days, sizeof(week_days_zero)))
        {
            if(cycle_plan_do_sleep(&p_plan->plan[i], false,true))
            {
                ezlog_i(TAG_LIGHT,"once helpsleepplan is done,report status to shadow");
                disable_light_sleep_plan(i,true);
            }
        }
        else
        {
            cycle_plan_do_sleep(&p_plan->plan[i],true,true);
        }
    }

    /* 唤醒计划判断是否需要执行*/
    p_plan = &g_bulb_param.wakeup_plan;
    for (i = 0; i < p_plan->count; i++)
    {
        if (0 == p_plan->plan[i].enabled)
        {
            continue;
        }

        ezlog_v(TAG_LIGHT, "helpsleep plan[%d], %d %d %d %d %d %d %d", i,
                p_plan->plan[i].week_days[0], p_plan->plan[i].week_days[1],
                p_plan->plan[i].week_days[2], p_plan->plan[i].week_days[3],
                p_plan->plan[i].week_days[4], p_plan->plan[i].week_days[5],
                p_plan->plan[i].week_days[6]);

        if (0 == memcmp(week_days_zero, p_plan->plan[i].week_days, sizeof(week_days_zero)))
        {
            if(cycle_plan_do_sleep(&p_plan->plan[i], false,false))
            {
                ezlog_i(TAG_LIGHT,"once wakeupplan is done,report status to shadow");
                disable_light_sleep_plan(i,true);
            }
        }
        else
        {
            cycle_plan_do_sleep(&p_plan->plan[i],true,false);
        }
    }

}


void light_plan_task(void *param)
{
    while (1)
    {
        ezos_time_t time_now = ezos_time(NULL);
        struct ezos_tm ptm_time_now = {0};

        if (NULL == ezos_localtime(&time_now, &ptm_time_now))
        {
            ezos_delay_ms(1000);
            continue;
        }

        if (ptm_time_now.tm_year + 1900 < 2022)
        {
            ezlog_v(TAG_LIGHT, "Correction time is not complete");
            ezos_delay_ms(1000);
            continue;
        }

        /* 定时计划判断是否有满足时间动作*/
        switch_plan_timer();

        /* 生物节律是否有满足时间条件*/
        biorhythm_plan_timer();

        /*唤醒入睡计划是否满足条件渐变关灯*/
        sleep_plan_timer();

        /*倒计时计划是否满足条件开关关灯*/
        countdown_timer();

        ezos_delay_ms(1000);
    }

    return;
}

int restart_light()
{
    ezlog_v(TAG_LIGHT, "function %s in.", __func__);
    printf("\n to_do DEBUG in line (%d) and function (%s)): \n ", __LINE__, __func__);

    return 0;
}

bool get_light_switch()
{
    return g_bulb_param.swit;
}

int get_light_brightness()
{
    return g_bulb_param.brightness;
}

int get_light_color_temperature()
{
    return g_bulb_param.cct;
}

int get_light_color_rgb()
{
    return g_bulb_param.rgb;
}

int get_light_mode()
{
    return g_bulb_param.mode;
}

void set_light_mode(int mode)
{
    g_bulb_param.mode = mode;
}

void* get_light_param()
{
    return &g_bulb_param;
}

void bulb_ctrl_init()
{
    ez_err_t rv = EZ_CORE_ERR_SUCC;

    /* 
    debug：
    此处需要实际从配置文件中读取灯泡的参数  
    */
    printf("\n to_do DEBUG in line (%d) and function (%s)): %d\n ", __LINE__, __func__, sizeof(g_bulb_param));


    g_led_current_param.brightness = g_bulb_param.brightness;
    g_led_current_param.cct_value = g_bulb_param.cct;
    g_led_current_param.rgb = g_bulb_param.rgb;

    
    const ez_char_t *bulb_ctrl_thread_name = "led_ctrl";
    rv = ezos_thread_create(&bulb_ctrl_thread, bulb_ctrl_thread_name, led_ctrl_Task,
                            (void *)bulb_ctrl_thread_name, 11 * 256, 10);
    if (EZ_CORE_ERR_SUCC != rv)
    {
        ezlog_e(TAG_APP, "creat bulb_ctrl thread error ,no memory");
    }

    
    const ez_char_t *bulb_scene_thread_name = "scene";
    rv = ezos_thread_create(&bulb_scene_thread, bulb_scene_thread_name, light_scene_task,
                            (void *)bulb_scene_thread_name, 2 * 1024, 5);

    const ez_char_t *bulb_plan_thread_name = "plan";
    rv = ezos_thread_create(&bulb_plan_thread, bulb_plan_thread_name, light_plan_task,
                            (void *)bulb_plan_thread_name, 4 * 1024, 5);

    if (EZ_CORE_ERR_SUCC != rv)
    {
        ezlog_e(TAG_APP, "creat bulb_ctrl thread error ,no memory");
    }
}

void bulb_ctrl_deinit()
{
    ezlog_e(TAG_APP, "total heap:%d, dram:%d", heap_caps_get_free_size(2), heap_caps_get_free_size(4));
    if (NULL != bulb_ctrl_thread)
	{
		ezlog_i(TAG_APP, "delete g_PthreadButton.\n");
		ezos_thread_destroy(bulb_ctrl_thread);
		bulb_ctrl_thread = NULL;
	}
			
	if (NULL != bulb_scene_thread)
	{
		ezlog_i(TAG_APP, "delete g_light_handle.\n");
		ezos_thread_destroy(bulb_scene_thread);
		bulb_scene_thread = NULL;
	}
    	if (NULL != bulb_plan_thread)
	{
		ezlog_i(TAG_APP, "delete g_light_handle.\n");
		ezos_thread_destroy(bulb_plan_thread);
		bulb_plan_thread = NULL;
	}
    ezlog_e(TAG_APP, "total heap:%d, dram:%d", heap_caps_get_free_size(2), heap_caps_get_free_size(4));
}
