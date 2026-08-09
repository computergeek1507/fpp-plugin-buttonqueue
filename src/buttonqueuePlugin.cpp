#include <fpp-pch.h>

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

#include <drogon/HttpAppFramework.h>
#include "common.h"
#include "settings.h"
#include "Plugin.h"
#include "Plugins.h"
#include "fpphttp.h"
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

class ButtonQueuePlugin : public FPPPlugin {
private:
    std::vector<int> _seqIdxQueue;
    std::string _currentFSEQ;
    std::string _currentAction;
    std::string _queuePlaylist;
    std::string _currentPlaylist;
    int _lastItem {-1};
    int _lastItem2 {-1};
    //bool _addStop {false};
    //bool _added {false};
    //Json::Value config;
    Command *_addSeqCommand {nullptr};
    Command *_clearQueueCommand {nullptr};

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
            args.push_back(CommandArg("index", "int", "Seq Index In Playlist(1 based)").setRange(1, 255).setDefaultValue("1"));
            args.push_back(CommandArg("duplicate", "bool", "Allow Duplicate Sequence").setDefaultValue("false"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            int seq_index = 0;
            bool duplicate = false;
            if (args.size() >= 1) {
                seq_index = std::stoi(args[0]);
            }
            if (args.size() >= 2) {
                duplicate = args[1] == "true";
            }
            plugin->AddSeqToQueue(seq_index, duplicate);
            return std::make_unique<Command::Result>("ButtonQueue Add Sequence");
        }
        ButtonQueuePlugin *plugin;
    };

    class ButtonQueueClearQueueCommand : public Command {
    public:
        ButtonQueueClearQueueCommand(ButtonQueuePlugin *p) : Command("ButtonQueue Clear Queue"), plugin(p) {
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            plugin->ClearQueue();
            return std::make_unique<Command::Result>("ButtonQueue Clear Queue");
        }
        ButtonQueuePlugin *plugin;
    };


    void registerCommand() {
        _addSeqCommand = new ButtonQueueAddSequenceCommand(this);
        _clearQueueCommand = new ButtonQueueClearQueueCommand(this);
        CommandManager::INSTANCE.addCommand(_addSeqCommand);
        CommandManager::INSTANCE.addCommand(_clearQueueCommand);
    }

    virtual std::function<bool()> shutdown() override {
        if (_addSeqCommand) {
            CommandManager::INSTANCE.removeCommand(_addSeqCommand);
            delete _addSeqCommand;
            _addSeqCommand = nullptr;
        }
        if (_clearQueueCommand) {
            CommandManager::INSTANCE.removeCommand(_clearQueueCommand);
            delete _clearQueueCommand;
            _clearQueueCommand = nullptr;
        }
        return nullptr;
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
        //if (action == "query_next" || action == "start" || action == "playing") 
        {
            _currentPlaylist = playlist["name"].asString();
            _currentAction = action;
            LogInfo(VB_PLUGIN, "currentPlaylist '%s'\n", _currentPlaylist.c_str());
            LogInfo(VB_PLUGIN, "action '%s'\n", action.c_str());

            if(!_seqIdxQueue.empty()) {
                if( (_currentPlaylist != _queuePlaylist && !_currentPlaylist.empty())) {
                    LogInfo(VB_PLUGIN, "insert queuePlaylist '%s'\n", _queuePlaylist.c_str());
                    LogInfo(VB_PLUGIN, "insert idx '%i'\n", _seqIdxQueue.front());
                // $url = "http://127.0.0.1/api/command/Insert%20Playlist%20Immediate/" . $remotePlaylistEncoded . "/" . $index . "/" . $index;
                    std::vector<std::string> keywords;
                    keywords.push_back(_queuePlaylist);
                    keywords.push_back(std::to_string(_seqIdxQueue.front()));
                    keywords.push_back(std::to_string(_seqIdxQueue.front()));
                    keywords.push_back("true");
                    _lastItem2 =_seqIdxQueue.front();
                    _seqIdxQueue.erase(_seqIdxQueue.begin());
                    CommandManager::INSTANCE.run("Insert Playlist Immediate",keywords);
                    //_addStop = false;
                }
                
                
            }else{
                LogInfo(VB_PLUGIN, "queue is empty\n");
                //_addStop = false;
                //_lastItem2 = -1;
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
        if (_currentPlaylist == _queuePlaylist && _lastItem2 == index /*&& !duplicate*/) {
            return;
        }
        auto sizeOfQueue = _seqIdxQueue.size();
        _seqIdxQueue.push_back(index);
        
    }

    void ClearQueue() {
        _seqIdxQueue.clear();
    }
    virtual void registerApis() override {
        auto handler = [this](const HttpRequestPtr& req, HttpCallback&& callback) {
            auto pieces = getPathPieces(req->path());
            if (pieces.size() > 1) {
                std::string p1 = pieces[1];
                if (p1 == "list") {
                    std::string v;
                    for (auto sd : _seqIdxQueue) {
                        v += std::to_string(sd) + ",";
                    }
                    callback(makeStringResponse(v, 200));
                    return;
                } else if (p1 == "play") {
                    callback(makeStringResponse(_queuePlaylist, 200));
                    return;
                }
            }
            callback(makeStringResponse("Not Found", 404));
        };
        FPPPlugins::registerPluginApi("/BUTTONQUEUE", handler, {drogon::Get}, true);
    }

    virtual void unregisterApis() override {
        FPPPlugins::unregisterPluginApi("/BUTTONQUEUE");
    }

};


extern "C" {
    FPPPlugin *createPlugin() {
        return new ButtonQueuePlugin();
    }
}
