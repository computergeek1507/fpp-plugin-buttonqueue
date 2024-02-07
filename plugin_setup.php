
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

var tplinkConfig = <? echo json_encode($pluginJson, JSON_PRETTY_PRINT); ?>;


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

</script>
</div>

</div>
<div>
<p>
<div class="col-auto">
        <div>
            <div class="row">
                <div class="col">
                    Last Messages:&nbsp;<input type="button" value="Get Playlist" class="buttons" onclick="RefreshPlaylist();">
                </div>
            </div>
            <div class="row">
                <div class="col">
                    <pre id="lastPlaylist" style='min-width:150px; margin:1px;min-height:300px;'></pre>
                </div>
            </div>
            <div class="row">
                <div class="col">
                    Last Messages:&nbsp;<input type="button" value="Refresh" class="buttons" onclick="RefreshLastMessages();">
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
