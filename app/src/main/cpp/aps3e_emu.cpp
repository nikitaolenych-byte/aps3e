// SPDX-License-Identifier: GPL-2.0-only

#include "emulator.h"
namespace ae{

    int boot_type;

    std::string boot_game_path;
    int boot_game_fd;

    ANativeWindow* window;
    int window_width;
    int window_height;

    std::string game_id;

    enum{
        STATUS_RUNNING=1,
        STATUS_STOPPED,
        STATUS_PAUSED,
        STATUS_REQUEST_PAUSE,
        STATUS_REQUEST_RESUME,
        STATUS_REQUEST_STOP,
    };

    pthread_mutex_t key_event_mutex;

    pthread_mutex_t emu_mutex;
    pthread_cond_t emu_cond;

    int emu_status=-1;



    void init();
    bool boot_game();

    void main_thr(){

        std::string tid=[]{
            std::stringstream ss;
            ss<<std::this_thread::get_id();
            return ss.str();
        }();
        LOGW("new thr: %s",tid.c_str());
        pthread_mutex_init(&key_event_mutex, NULL);

        pthread_mutex_init(&emu_mutex, NULL);
        pthread_cond_init(&emu_cond, NULL);

        init();

        bool boot_ok=boot_game();

        emu_status=STATUS_RUNNING;
        while (true){

            if (emu_status == STATUS_REQUEST_RESUME) {
                pthread_mutex_lock(&emu_mutex);
                Emu.Resume();
                emu_status = STATUS_RUNNING;
                pthread_cond_signal(&emu_cond);

                pthread_mutex_unlock(&emu_mutex);
            }
            if (emu_status == STATUS_REQUEST_PAUSE) {
                pthread_mutex_lock(&emu_mutex);
                Emu.Pause(false,false);
                emu_status = STATUS_PAUSED;
                pthread_cond_signal(&emu_cond);
                pthread_mutex_unlock(&emu_mutex);
            }
            if (emu_status == STATUS_REQUEST_STOP) {
                pthread_mutex_lock(&emu_mutex);
                if(boot_ok) Emu.Kill();
                emu_status = STATUS_STOPPED;
                pthread_cond_signal(&emu_cond);
                pthread_mutex_unlock(&emu_mutex);
                usleep(10);
                break;
            }
            usleep(10);
        }

        pthread_mutex_destroy(&key_event_mutex);
        pthread_mutex_destroy(&emu_mutex);
        pthread_cond_destroy(&emu_cond);
    }

    void key_event(int key_code,bool pressed,int value){
        pthread_mutex_lock(&key_event_mutex);
        {
            auto* pad_thr=g_fxo->try_get<named_thread<pad_thread>>();
            if(pad_thr){
                auto xx=pad_thr->get_handlers().at(pad_handler::keyboard);
                std::shared_ptr<AndroidVirtualPadHandler> padh=std::dynamic_pointer_cast<AndroidVirtualPadHandler>(xx);
                padh->Key(static_cast<u32>(key_code), static_cast<bool>(pressed),value);
            }
        }
        pthread_mutex_unlock(&key_event_mutex);
    }
    bool is_running(){
        return emu_status==STATUS_RUNNING;
    }
    bool is_paused(){
        return emu_status==STATUS_PAUSED;
    }
    void pause(){
        pthread_mutex_lock(&emu_mutex);
        emu_status=STATUS_REQUEST_PAUSE;
        while(emu_status==STATUS_REQUEST_PAUSE)
            pthread_cond_wait(&emu_cond,&emu_mutex);
        pthread_mutex_unlock(&emu_mutex);
    }
    void resume(){
        pthread_mutex_lock(&emu_mutex);
        emu_status=STATUS_REQUEST_RESUME;
        while(emu_status==STATUS_REQUEST_RESUME)
            pthread_cond_wait(&emu_cond,&emu_mutex);
        pthread_mutex_unlock(&emu_mutex);
    }
    void quit(){
        pthread_mutex_lock(&emu_mutex);
        emu_status=STATUS_REQUEST_STOP;
        while(emu_status==STATUS_REQUEST_STOP)
            pthread_cond_wait(&emu_cond,&emu_mutex);
        pthread_mutex_unlock(&emu_mutex);
    }

    /** RPCS3 emulator has functions it desires to call from the GUI at times. Initialize them in here. */
    EmuCallbacks create_emu_cb()
    {
        EmuCallbacks callbacks{};

        callbacks.on_run    = [](bool /*start_playtime*/) {
            PRE("on_run");
        };

        callbacks.on_pause  = []() {
            PRE("on_pause");
        };
        callbacks.on_resume = []() {
            PRE("on_resume");
        };

        callbacks.update_emu_settings = [](){PRE("update_emu_settings");};
        callbacks.save_emu_settings = [](){PRE("save_emu_settings");};
        callbacks.init_kb_handler = [](){PRE("init_kb_handler");
            g_fxo->init<KeyboardHandlerBase, NullKeyboardHandler>(Emu.DeserialManager());
        };

        callbacks.init_mouse_handler = []()
        {
            PRE("init_mouse_handler");
#if 0
            mouse_handler handler = g_cfg.io.mouse;

		if (handler == mouse_handler::null)
		{
			switch (g_cfg.io.move)
			{
			case move_handler::mouse:
				LOGW("handler = mouse_handler::basic");
				break;
			case move_handler::raw_mouse:
				LOGW("handler = mouse_handler::raw");
				break;
			default:
				break;
			}
		}

		switch (handler)
		{
		case mouse_handler::null:
		{
			g_fxo->init<MouseHandlerBase, NullMouseHandler>(Emu.DeserialManager());
			break;
		}
		case mouse_handler::basic:
		{
			/*basic_mouse_handler* ret = g_fxo->init<MouseHandlerBase, basic_mouse_handler>(Emu.DeserialManager());
			ensure(ret);
			ret->moveToThread(get_thread());
			ret->SetTargetWindow(m_game_window);
			*/break;
		}
		case mouse_handler::raw:
		{
			g_fxo->init<MouseHandlerBase, raw_mouse_handler>(Emu.DeserialManager());
			break;
		}
		}
#else
            g_fxo->init<MouseHandlerBase, NullMouseHandler>(Emu.DeserialManager());
#endif
        };

        callbacks.init_pad_handler = [](std::string_view title_id)
        {
            PRE("init_pad_handler");
            aps3e_log.warning("init_pad_handler: title_id=%s", title_id);

            ensure(g_fxo->init<named_thread<pad_thread>>(nullptr, nullptr, title_id));
            qt_events_aware_op(0, [](){ return !!pad::g_started; });
        };

        callbacks.get_audio = []() -> std::shared_ptr<AudioBackend>
        {
            PRE("get_audio");
            std::shared_ptr<AudioBackend> result =std::make_shared<NullAudioBackend>();
            switch (g_cfg.audio.renderer.get())
            {
                case audio_renderer::null: result = std::make_shared<NullAudioBackend>(); break;
                case audio_renderer::cubeb: result = std::make_shared<CubebBackend>(); break;
#ifdef HAVE_FAUDIO
                    case audio_renderer::faudio: result = std::make_shared<FAudioBackend>(); break;
#endif
            }

            if (!result->Initialized())
            {
                PRE("!result->Initialized()");
                // Fall back to a null backend if something went wrong
                //sys_log.error("Audio renderer %s could not be initialized, using a Null renderer instead. Make sure that no other application is running that might block audio access (e.g. Netflix).", result->GetName());
                result = std::make_shared<NullAudioBackend>();
            }
            return result;
        };

        callbacks.get_audio_enumerator = [](u64 renderer) -> std::shared_ptr<audio_device_enumerator>
        {
            PRE("get_audio_enumerator");
            switch (static_cast<audio_renderer>(renderer))
            {
                case audio_renderer::null: return std::make_shared<null_enumerator>();
#ifdef _WIN32
                    case audio_renderer::xaudio: return std::make_shared<xaudio2_enumerator>();
#endif
                case audio_renderer::cubeb: return std::make_shared<cubeb_enumerator>();
#ifdef HAVE_FAUDIO
                    case audio_renderer::faudio: return std::make_shared<faudio_enumerator>();
#endif
                default: return std::make_shared<null_enumerator>();
            }
        };

        callbacks.get_image_info = [](const std::string& filename, std::string& sub_type, s32& width, s32& height, s32& orientation) -> bool
        {
            PRE("get_image_info");
            return false;
        };

        callbacks.get_scaled_image = [](const std::string& path, s32 target_width, s32 target_height, s32& width, s32& height, u8* dst, bool force_fit) -> bool
        {
            PRE("get_scaled_image");
            return false;
        };

        callbacks.resolve_path = [](std::string_view sv)
        {
            LOGW("resolve_path: %s",sv.data());
            if(sv.ends_with("/"sv)){
                sv.remove_suffix(1);
                return std::string{sv};
            }
            return std::string{sv};
            // May result in an empty string if path does not exist
            //return QFileInfo(QString::fromUtf8(sv.data(), static_cast<int>(sv.size()))).canonicalFilePath().toStdString();
        };

        callbacks.on_install_pkgs = [](const std::vector<std::string>& pkgs)
        {
            PRE("on_install_pkgs");
            bool result=true;
            if(pkgs[0][0]==':') {
                for(const auto& pkg : pkgs){
                    if(!aps3e_util::install_pkg(*Emu.GetIsoFs(),pkg)){
                        result=false;
                    }
                }
            }
            else{
                for (const auto& pkg : pkgs)
                {
                    if(!aps3e_util::install_pkg(pkg.c_str())){
                        result=false;
                    }
                }
            }
            return result;
        };

        callbacks.try_to_quit = [](bool force_quit, std::function<void()> on_exit) -> bool
        {
            PRE("try_to_quit");
            if (on_exit)
            {
                on_exit();
            }
            /*if (force_quit)
            {
                if (on_exit)
                {
                    on_exit();
                }

                quit();
                return true;
            }*/
            return true;
        };
        callbacks.call_from_main_thread = [](std::function<void()> func, atomic_t<u32>* wake_up)
        {
            PRE("call_from_main_thread");

            func();

            if (wake_up)
            {
                *wake_up = true;
                wake_up->notify_one();
            }
        };

        callbacks.init_gs_render = [](utils::serial* ar)
        {
            PRE("init_gs_render");
            switch (g_cfg.video.renderer.get())
            {
                case video_renderer::null:
                {
                    LOGE("video_renderer::null");
                    break;
                }
                case video_renderer::vulkan:
                {
                    g_fxo->init<rsx::thread, named_thread<VKGSRender>>(ar);
                    break;
                }
            }
        };

        callbacks.get_camera_handler = []() -> std::shared_ptr<camera_handler_base>
        {
            PRE("get_camera_handler");
            /*switch (g_cfg.io.camera.get())
            {
            case camera_handler::null:
            case camera_handler::fake:
            {
                return std::make_shared<null_camera_handler>();
            }
            case camera_handler::qt:
            {
                fmt::throw_exception("Headless mode can not be used with this camera handler. Current handler: %s", g_cfg.io.camera.get());
            }
            }*/
            return std::make_shared<null_camera_handler>();
        };

        callbacks.get_music_handler = []() -> std::shared_ptr<music_handler_base>
        {
            PRE("get_music_handler");
            switch (g_cfg.audio.music.get())
            {
                case music_handler::null:
                {
                    return std::make_shared<null_music_handler>();
                }
                case music_handler::qt:
                {
                    return std::make_shared<android_music_handler>();
                }
            }
        };

        callbacks.get_msg_dialog                 = []() -> std::shared_ptr<MsgDialogBase> {
            PRE("get_msg_dialog");
            return std::make_shared<dummy_msg_dialog>(); };
        callbacks.get_osk_dialog                 = []() -> std::shared_ptr<OskDialogBase> {
            PRE("get_osk_dialog");
            return std::make_shared<dummy_osk_dialog>(); };
        callbacks.get_save_dialog                = []() -> std::unique_ptr<SaveDialogBase> {
            PRE("get_save_dialog");
            return std::make_unique<android_save_dialog>(); };
        callbacks.get_trophy_notification_dialog = []() -> std::unique_ptr<TrophyNotificationBase> {
            PRE("get_trophy_notification_dialog");
            return std::make_unique<android_trophy_notification>(); };
        callbacks.get_sendmessage_dialog=[]()->std::shared_ptr<SendMessageDialogBase>{
            PRE("get_sendmessage_dialog");
            return std::make_shared<dummy_send_message_dialog>();
        };
        callbacks.get_recvmessage_dialog=[]()->std::shared_ptr<RecvMessageDialogBase>{
            PRE("get_recvmessage_dialog");
            return std::make_shared<dummy_recv_message_dialog>();
        };
        callbacks.on_stop   = []() {
            PRE("on_stop");
        };
        callbacks.on_ready  = []() {
            PRE("on_ready");
        };
        callbacks.on_emulation_stop_no_response = [](std::shared_ptr<atomic_t<bool>> closed_successfully, int /*seconds_waiting_already*/)
        {
            PRE("on_emulation_stop_no_response");
        };

        callbacks.on_save_state_progress = [](std::shared_ptr<atomic_t<bool>>, stx::shared_ptr<utils::serial>, stx::atomic_ptr<std::string>*, std::shared_ptr<void>)
        {
            PRE("on_save_state_progress");
        };

        callbacks.enable_disc_eject  = [](bool) {
            PRE("enable_disc_eject");

        };
        callbacks.enable_disc_insert = [](bool) {
            PRE("enable_disc_insert");
        };

        callbacks.on_missing_fw = []() {
            PRE("on_missing_fw");
        };

        callbacks.handle_taskbar_progress = [](s32, s32) {
            PRE("handle_taskbar_progress");
        };

        callbacks.get_localized_string    = [](localized_string_id id, const char* args) -> std::string {
            PRE("get_localized_string");

            std::string str = localized_strings[static_cast<size_t>(id)];
            if(auto it=str.find("%0");it!=std::string::npos) {
                str.replace(it, 2, args);
            }
            return str;
        };
        /*callbacks.get_localized_u32string = [](localized_string_id, const char*) -> std::u32string {
            PRE("get_localized_u32string");
            return {}; };*/

        callbacks.play_sound = [](const std::string&){
            PRE("play_sound");

        };
        callbacks.add_breakpoint = [](u32 /*addr*/){
            PRE("add_breakpoint");

        };

        return callbacks;
    }
    void init(){

        const char* enable_log=getenv("APS3E_ENABLE_LOG");
        if(enable_log&&strcmp(enable_log,"true")==0){
            static std::unique_ptr<logs::listener> log_file = logs::make_file_listener(std::string(getenv("APS3E_LOG_DIR"))+"/rp3_log.txt", 1024*1024*1024);
        }

        prctl(PR_SET_TIMERSLACK, 1, 0, 0, 0);

        // Initialize TSC freq (in case it isn't)
        static_cast<void>(utils::get_tsc_freq());

        // Initialize thread pool finalizer (on first use)
        static_cast<void>(named_thread("", [](int) {}));

        struct rlimit64 rlim;
        rlim.rlim_cur = 4096;
        rlim.rlim_max = 4096;

        if (setrlimit64(RLIMIT_NOFILE, &rlim) != 0)
        {
            LOGE("Failed to get max open file limit");
        }
        else{
            LOGW("RLIMIT_NOFILE: rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }

        rlim.rlim_cur = 0x80000000;
        rlim.rlim_max = 0x80000000;
        if (setrlimit64(RLIMIT_MEMLOCK, &rlim) != 0)
        {
            LOGE("Failed to get RLIMIT_MEMLOCK size");
        }
        else{
            LOGW("RLIMIT_MEMLOCK: rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }

        /*if(prlimit64(0,RLIMIT_MEMLOCK,NULL,&rlim)==0){
            LOGW("prlimit64(RLIMIT_MEMLOCK): rlim_max 0x%x ,rlim_cur 0x%x",rlim.rlim_max,rlim.rlim_cur);
        }*/

        EmuCallbacks callbacks=create_emu_cb();

        callbacks.get_gs_frame = [=]() -> std::unique_ptr<GSFrameBase>
        {
            PRE("get_gs_frame");
            return std::unique_ptr<android_gs_frame>(new android_gs_frame(window,window_width,window_height));
        };

        Emu.SetCallbacks(std::move(callbacks));
        Emu.SetHasGui(true);

        Emu.Init();
    }

    bool boot_game(){

        //Emu.CallFromMainThread([=]()
        {
            Emu.SetForceBoot(true);

            const char* config_path=getenv("APS3E_CUSTOM_CONFIG_YAML_PATH");
            const cfg_mode config_mode = config_path?cfg_mode::custom:cfg_mode::global ;

            aps3e_log.warning("iso_fd: %d",boot_game_fd);
            const game_boot_result error =boot_type==BOOT_TYPE_WITH_PATH? Emu.BootGame(boot_game_path, game_id, true, config_mode, config_path?:"")
                                                             :Emu.BootISO(":PS3_GAME/USRDIR/EBOOT.BIN",game_id,boot_game_fd,config_mode, config_path?:"");
            LOGW("game_boot_result %d",error);
            return error==game_boot_result::no_errors;
        }//);
    }
}
