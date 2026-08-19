# fpp-plugin-buttonqueue
FPP Button Queue Plugin

Lets viewers "request" a sequence from a chosen playlist without interrupting the show that's
currently running. Wire up buttons (physical GPIO buttons, MQTT, virtual remotes, keypads, etc.)
to FPP Commands, and requested sequences get inserted and played as soon as the current item
finishes, before the main playlist continues on to its next item, in the order they were
requested.

Add Plugin URL to FPP
https://raw.githubusercontent.com/computergeek1507/fpp-plugin-buttonqueue/main/pluginInfo.json

## Usage
1. On the **Button Queue - Configuration** page, pick the Playlist that holds the requestable
   sequences and click **Save**.
2. Create an FPP Command (Command Preset, GPIO Input, MQTT, virtual remote, etc.) per requestable
   sequence that runs the **ButtonQueue Add Sequence** command, passing the 1-based index of that
   sequence within the playlist and an **Allow Duplicate Sequence** option.
3. **Allow Duplicate Sequence** controls whether the same index can have more than one request
   waiting in the queue at once: `false` ignores repeat requests for a sequence already queued,
   `true` allows stacking multiple requests for it. Either way, a request is ignored if that same
   sequence was just inserted from the queue and is still the item currently playing, so an
   accidental double-press can't insert it twice back-to-back.
4. Optionally, create a command that runs **ButtonQueue Clear Queue** to empty out any pending
   requests.

The Configuration page also shows the currently configured playlist and the sequence indexes
currently queued and waiting to play.
