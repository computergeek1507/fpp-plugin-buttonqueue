
<?
function returnIfExists($json, $setting) {
    if ($json == null) {
        return "";
    }
    if (array_key_exists($setting, $json)) {
        return $json[$setting];
    }
    return "";
}

function convertAndGetSettings() {
    global $settings;
        
    $cfgFile = $settings['configDirectory'] . "/plugin.buttonqueue.json";
    if (file_exists($cfgFile)) {
        $j = file_get_contents($cfgFile);
        $json = json_decode($j, true);
        return $json;
    }
    $j = "[]";
    return json_decode($j, true);
}

$pluginJson = convertAndGetSettings();
?>


<div id="global" class="settings">
<fieldset>
<legend>FPP Button Queue Config</legend>

<script>

var buttonQueueConfig = <? echo json_encode($pluginJson, JSON_PRETTY_PRINT); ?>;


function RefreshLastMessages() {
    $.get('api/plugin-apis/BUTTONQUEUE/list', function (data) {
          $("#lastMessages").text(data);
        }
    );
}

function RefreshPlaylist() {
    $.get('api/plugin-apis/BUTTONQUEUE/play', function (data) {
          $("#lastPlaylist").text(data);
        }
    );
}

function getPlaylists() {
    $.get('/api/playlists', function (data) {
    var playlistOptions = '';
    data.forEach(playlist => {
      if(playlist === buttonQueueConfig["playlist"]) {
        playlistOptions += '<option selected value="' + playlist + '">' + playlist + '</option>';
      }else {
        playlistOptions += '<option value="' + playlist + '">' + playlist + '</option>';
      }
    });
    $('#remotePlaylistSelect').html(playlistOptions);
  })
}

function SaveButtonQueue() {
    newserialeventConfig = { "playlist": ''};
    newserialeventConfig["playlist"] = document.getElementById("remotePlaylistSelect").value;

    var data = JSON.stringify(newserialeventConfig);
    $.ajax({
        type: "POST",
	url: 'api/configfile/plugin.buttonqueue.json',
        dataType: 'json',
        async: false,
        data: data,
        processData: false,
        contentType: 'application/json',
        success: function (data) {
           SetRestartFlag(2);
        }
    });
}


$(document).ready(function() {
    getPlaylists();
              });
</script>

<div class="col-auto">
        <div>
            <div class="input-group">
                <select class="form-control" id="remotePlaylistSelect" name="remotePlaylistSelect"></select>
                <input type="button" value="Save" class="buttons genericButton" onclick="SaveButtonQueue();">
            </div>
            <div class="row">
                <div class="col">
                    Using Playlist:&nbsp;<input type="button" value="Get Playlist" class="buttons" onclick="RefreshPlaylist();">
                </div>
            </div>
            <div class="row">
                <div class="col">
                    <pre id="lastPlaylist" style='min-width:150px; margin:1px;min-height:300px;'></pre>
                </div>
            </div>
            <div class="row">
                <div class="col">
                    Current Indexes:&nbsp;<input type="button" value="Refresh" class="buttons" onclick="RefreshLastMessages();">
                </div>
            </div>
            <div class="row">
                <div class="col">
                    <pre id="lastMessages" style='min-width:150px; margin:1px;min-height:300px;'></pre>
                </div>
            </div>
        </div>
    </div>
<p>
</div>
</div>


</fieldset>
</div>
