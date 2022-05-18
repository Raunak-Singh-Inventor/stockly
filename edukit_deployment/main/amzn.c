#include "core2forAWS.h"
#include "freertos/task.h" 

#include "includes/amzn/amzn_functions.hpp"

char* amzn_tag = "AMZN";

void amzn_display() {
    lv_obj_clean(lv_scr_act());

    LV_FONT_DECLARE(jetbrains_mono_bold_60px)
    static lv_style_t st2;
    lv_style_init(&st2);
    lv_style_set_text_font(&st2, LV_STATE_DEFAULT, &jetbrains_mono_bold_60px);

    
    LV_FONT_DECLARE(jetbrains_mono_semibold_30px)
    static lv_style_t st3;
    lv_style_init(&st3);
    lv_style_set_text_font(&st3, LV_STATE_DEFAULT, &jetbrains_mono_semibold_30px);

    int prices[] = {100,90,100,50,130,150,100};
    float price = amzn_loop(prices);
    
    ESP_LOGI(amzn_tag, "Predicted Price: $%.2f", price);
    
    lv_obj_t * label4 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label4, true);
    lv_label_set_text(label4, "#FFAC1C AMZN");
    lv_obj_align(label4, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 10);
    lv_obj_add_style(label4, LV_LABEL_PART_MAIN , &st2);

    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
    char* text2[32]; 
    sprintf(text2, "Current Price: \n   $%d", prices[6]);
    lv_label_set_text(label2, text2);
    lv_obj_align(label2, NULL, LV_ALIGN_IN_LEFT_MID, 15, -20);
    lv_obj_add_style(label2, LV_LABEL_PART_MAIN , &st3);

    lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
    char* text3[32]; 
    lv_label_set_recolor(label3, true);
    if(price < prices[6]) {
        sprintf(text3, "Predicted Price: \n  #FF634 $%.2f", price);
    } else {
        sprintf(text3, "Predicted Price: \n  #097969 $%.2f", price);
    }
    lv_label_set_text(label3, text3);
    lv_obj_align(label3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 15, -45);
    lv_obj_add_style(label3, LV_LABEL_PART_MAIN , &st3);
}