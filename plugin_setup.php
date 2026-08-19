
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

<div class="alert alert-info" style="margin-top:10px;">
    <b>What this plugin does:</b> Button Queue lets viewers (or you) "request" a sequence without
    interrupting the currently playing playlist. You wire up buttons - physical GPIO buttons, MQTT,
    virtual remotes, keypads, etc. - to FPP Commands, and requested sequences get inserted and
    played as soon as the currently playing item finishes, before the main playlist continues on
    to its next item, in the order they were requested.
    <br><br>
    <b>How to set it up:</b>
    <ol style="margin-bottom:0;">
        <li>Below, pick the Playlist that contains the sequences you want people to be able to request, then click <b>Save</b>.</li>
        <li>Create an FPP Command (Command Presets, GPIO Input, MQTT, etc.) that runs the <b><i>ButtonQueue Add Sequence</i></b> command,
            passing the 1-based <b>index</b> of the sequence within that playlist and an <b>Allow Duplicate Sequence</b> option.
            One command/button per requestable sequence.</li>
        <li><b>Allow Duplicate Sequence</b> controls whether the same index can have more than one request waiting in the queue
            at once: leave it <b>false</b> so repeated presses of the same button before it plays are ignored, or set it to
            <b>true</b> to let the same sequence be queued up multiple times in a row. Either way, a button press is always
            ignored if that same sequence was just the last thing inserted from the queue and is still the item playing,
            so accidental double-presses can't insert it twice back-to-back.</li>
        <li>Optionally, create another command that runs <b><i>ButtonQueue Clear Queue</i></b> to empty out any pending requests.</li>
        <li>Use <b>Get Playlist</b> and <b>Refresh</b> below to confirm the configured playlist and see which sequence indexes are
            currently queued and waiting to play.</li>
    </ol>
</div>

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
