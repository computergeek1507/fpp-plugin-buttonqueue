#include "fpp-pch.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstring>

#include <istream>
#include <ostream>

#include <iostream> 

#include <thread> 

#include <vector>

#include <unistd.h>
#include <termios.h>
#include <chrono>
#include <thread>
#include <cmath>

#include <httpserver.hpp>
#include "common.h"
#include "settings.h"
#include "Plugin.h"
#include "Plugins.h"
#include "log.h"
#include "MultiSync.h"

#if __has_include("channeloutput/ChannelOutputSetup.h")
#include "channeloutput/ChannelOutputSetup.h"
#elif __has_include("channeloutput/channeloutput.h")
#include "channeloutput/channeloutput.h"
#endif

#include "fppversion_defines.h"

#include "commands/Commands.h"

//#include "ButtonQueueItem.h"

class ButtonQueuePlugin : public FPPPlugin, public httpserver::http_resource {
private:
    std::vector<int> _seqIdxQueue;
    std::string _currentFSEQ;
    std::string _currentAction;
    std::string _queuePlaylist;
    std::string _currentPlaylist;
    int _lastItem {-1};
    //Json::Value config;

public:
    ButtonQueuePlugin() : FPPPlugin("fpp-plugin-ButtonQueue") {
        LogInfo(VB_PLUGIN, "Initializing ButtonQueue Plugin\n");
        readFiles();
        registerCommand();
    }
    virtual ~ButtonQueuePlugin() {
    }

    std::string GetQueuePlaylist() const {
        return _queuePlaylist;
    }

    class ButtonQueueAddSequenceCommand : public Command {
    public:
        ButtonQueueAddSequenceCommand(ButtonQueuePlugin *p) : Command("ButtonQueue Add Sequence"), plugin(p) {
            args.push_back(CommandArg("index", "int", "seq index").setDefaultValue("0"));
            args.push_back(CommandArg("duplicate", "bool", "Allow Duplicate Sequence").setRange(0, 255).setDefaultValue("false"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            int seq_index = 0;
            bool duplicate = false;
            if (args.size() >= 1) {
                seq_index = std::stoi(args[0]);
            }
            if (args.size() >= 2) {
                duplicate = args[1]=="true";
            }
            plugin->AddSeqToQueue(seq_index, duplicate);
            return std::make_unique<Command::Result>("ButtonQueue Add Sequence");
        }
        ButtonQueuePlugin *plugin;
    };

    void registerCommand() {
        CommandManager::INSTANCE.addCommand(new ButtonQueueAddSequenceCommand(this));
    }

    virtual void modifySequenceData(int ms, uint8_t *seqData) override {
        try
        {
            //sendChannelData(seqData);
        }
        catch(std::exception const& ex)
        {
            std::cout << ex.what();
        }
    }

    virtual void playlistCallback(const Json::Value &playlist, const std::string &action, const std::string &section, int item) {
        //if (settings["Start"] == "PlaylistStart" && action == "start") {
            //EnableButtonQueueItems();
        //}
        //if (action == "playing" || action == "start") 
        {
            _currentPlaylist = playlist["name"].asString();
            _currentAction = action;
            LogInfo(VB_PLUGIN, "currentPlaylist '%s'\n", _currentPlaylist.c_str());
            LogInfo(VB_PLUGIN, "action '%s'\n", action.c_str());

            if(!_seqIdxQueue.empty()) {
                if( _currentPlaylist != _queuePlaylist && !_currentPlaylist.empty()) {
                    LogInfo(VB_PLUGIN, "insert queuePlaylist '%s'\n", _queuePlaylist.c_str());
                    LogInfo(VB_PLUGIN, "insert idx '%i'\n", _seqIdxQueue.front());
                // $url = "http://127.0.0.1/api/command/Insert%20Playlist%20Immediate/" . $remotePlaylistEncoded . "/" . $index . "/" . $index;
                    std::vector<std::string> keywords;
                    keywords.push_back(_queuePlaylist);
                    keywords.push_back(std::to_string(_seqIdxQueue.front()));
                    keywords.push_back(std::to_string(_seqIdxQueue.front()));
                     keywords.push_back("false");
                    _seqIdxQueue.erase(_seqIdxQueue.begin());
                    CommandManager::INSTANCE.run("Insert Playlist Immediate",keywords);
                }
            }else{
                LogInfo(VB_PLUGIN, "queue is empty\n");
            }
            _lastItem = item;
        }
    }

    void sendChannelData(unsigned char *data) {
        
    }
    
    void readFiles()
    {
        try {
            std::string configLocation = FPP_DIR_CONFIG("/plugin.buttonqueue.json");
            if (FileExists(configLocation)) {
                Json::Value root;
                if (LoadJsonFromFile(configLocation, root)) {
                    _queuePlaylist = root["playlist"].asString();
                    LogInfo(VB_PLUGIN, "ButtonQueue Plugin Playlist %s\n", _queuePlaylist.c_str());
                }
            }else{
                LogErr(VB_PLUGIN, "plugin.buttonqueue.json not found\n");
            }
        } catch (...) {
            LogErr(VB_PLUGIN, "reading plugin.buttonqueue.json threw\n");
        }
    }  

    void AddSeqToQueue(int index, bool duplicate) {
        if (std::count(_seqIdxQueue.begin(), _seqIdxQueue.end(), index) && !duplicate) {
            return;
        }
        _seqIdxQueue.push_back(index);
    }
    virtual HTTP_RESPONSE_CONST std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request &req) override {
        
        if (req.get_path_pieces().size() > 1) {
            std::string p1 = req.get_path_pieces()[1];
            if (p1 == "list") {
                std::string v;
                for (auto sd : _seqIdxQueue) {
                    v += std::to_string(sd) + "\n";
                }
                return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(v, 200));
            } else if (p1 == "play") {
                return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(_queuePlaylist, 200));
            } 
        }
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("Not Found", 404));
    }
    void registerApis(httpserver::webserver *m_ws) override {
        m_ws->register_resource("/BUTTONQUEUE", this, true);
    }

};


extern "C" {
    FPPPlugin *createPlugin() {
        return new ButtonQueuePlugin();
    }
}
