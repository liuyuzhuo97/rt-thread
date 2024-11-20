
main_thread
->  init console
    ->  init uart
        print something
        receive cmds to change pose
        ->  send pose_change_msg(send velocity + acceleration + direection)
            sleep few ms
->  init leds
    ->  init gpio output
->  init mpu6050
    ->  init i2c/spi
        ->  polling mode/enable interrupt
        init mpu6050
        ->  read id
            configure mpu6050 mode
            enable sampling(stream mode/fifo mode)
->  init motor
    ->  init pwm0~3
        ->  init timer0~3
            pwm0~3 output 0
->  init watchdog                                                   ->  save what when timeout?
    ->  enable watchdog_timer
        ->  feed watchdog

->  init pose status
    ->  create pose_update_thread
        ->  receive pose_change_msg(receive velocity + acceleration + direection)
            ->  update target_pose
                ->  integrate velocity + acceleration + direection to get target v + w + position + orientation         ->  how to parse to pose???
        ->  receive pose_update_msg(receive sampled accel + gyro)
            ->  update global_pose                                  ->  filter here?
                ->  update global_v
                ->  update global_w
                ->  update global_position
                    ->  integrate velocity
                        ->  integrate acceleration
                ->  update global_orientation(in quat type)
                    ->  update global_quat
                    ->  update global_yaw                           ->  how to calculate without magnetometer???
                        update global_roll && global pitch
                        ->  calculate with accel
                            calculate with gyro
                            ->  filter with combined filter(high-pass + low-pass)
->  init pid status                                                 ->  position + velocity?
    ->  create pid_thread
        ->  receive pose_update_msg && get global_pose && get target_pose
            ->  update pid output                                   -> calculate a lot
                -> pwm0~3 change duty
->  enable sampling_timer
    ->  sampling accel/gyro                                         ->  or filter here?
        ->  convert to quat
            ->  send pose_update_msg(send sampled accel + gyro)
