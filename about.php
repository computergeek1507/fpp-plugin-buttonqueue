  <div style="margin:0 auto;"> <br />
    <fieldset style="padding: 10px; border: 2px solid #000;">
      <legend>button queue Plugin Info</legend>
      <div style="overflow: hidden; padding: 10px;">
    <div>
      <p>
        <b>What it does:</b> Button Queue lets you "request" a sequence from a chosen playlist without
        interrupting the show that's currently running. Trigger the <b>ButtonQueue Add Sequence</b> FPP
        Command (from a physical button, GPIO input, MQTT, virtual remote, keypad, etc.) with the
        1-based index of the sequence in that playlist, and it will be inserted and played as soon
        as the current item finishes, before the main playlist continues on, in the order requested. The command's <b>Allow Duplicate Sequence</b> option controls whether that
        same index can be queued more than once at a time (false ignores repeat requests already
        waiting in the queue, true allows stacking them); a request is always ignored if that same
        sequence is the one that was just inserted and is still playing. The <b>ButtonQueue Clear
        Queue</b> command empties any pending requests.
      </p>
      <p>
        Configure which playlist is used, and watch the current queue, on the
        <b>Button Queue - Configuration</b> page.
      </p>
      <div id='credits'>
        <b>button queue Plugin Developed By:</b><br />
		<br />
        Scott Hanson (computergeek1507)<br />
		<br />
        <a href='https://github.com/computergeek1507/fpp-plugin-buttonqueue'>Git Repository</a><br>
        <a href='https://github.com/computergeek1507/fpp-plugin-buttonqueue/issues'>Bug Reporter</a><br>
		<br />
      </div>
    </div>
    </fieldset>
  </div>
