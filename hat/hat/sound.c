#include "sound.h"
#include "pio.h"
#include "panic.h"
#include "target.h"
#include <math.h>

#define BUZZER_PWM_FREQ_HZ 100000
#define MELODY_MCDONALDS "E6/16 E6/16 C6/16 G5/16 E6/16 E6/16 C6/16 G5/16 E6/16 E6/16 C6/16 G5/16"

static mmelody_obj_t mmelody_dev;
static mmelody_t mmelody;
static pwm_t buzzer_pwm;

static const pwm_cfg_t buz_pwm_cfg = {
    .pio = PIEZO_PIO,  // Must match your hardware
    .period = PWM_PERIOD_DIVISOR(BUZZER_PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR(BUZZER_PWM_FREQ_HZ, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static void buzzer_callback(void *data, uint8_t note, uint8_t volume) {
    (void)data;
    (void)volume;
    
    if (note < 60) {
        pwm_stop(buzzer_pwm);
    } else {
        pwm_start(buzzer_pwm);
        uint32_t frequency = 440.0 * pow(2.0, (note - 69.0) / 12.0);
        pwm_frequency_set(buzzer_pwm, frequency);
    }
}

void sound_init(void) {
    buzzer_pwm = pwm_init(&buz_pwm_cfg);
    if (!buzzer_pwm) panic(LED_ERROR_PIO, 10);
    
    mmelody = mmelody_init(&mmelody_dev, 200, buzzer_callback, NULL);
    mmelody_speed_set(mmelody, 200);
    pwm_start(buzzer_pwm);
}

void sound_play_mcdonalds(void) {
    mmelody_play(mmelody, MELODY_MCDONALDS);
}

void sound_update(void) {
    mmelody_update(mmelody);
}

