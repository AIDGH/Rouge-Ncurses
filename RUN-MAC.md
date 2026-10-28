# Rouge-Ncurses on macOS — 2026-10-27

Double-click **Run-Rouge-Dungeon.command** in this folder. It rebuilds changed sources and launches the native Apple Silicon executable, requests a 180 × 55 terminal, and stores diagnostics in `build-macos/logs/`.

First choose **ORIGIN** (the original worms, art, map symbols and HUD) or **NEW / LANTERN** (castle, connected line art and new HUD). Arrows or mouse hover preview each style; Enter or click selects it. Press **V** on the main menu to choose again.

Choose **ENTER THE DUNGEON → New Game**. Use a registered hero to save progress; guest runs are temporary (their score can appear as “You”).

- WASD / arrows: movement. Q / Z / C / Page Up: diagonals.
- Space: attack, then direction for a ranged weapon. E: repeat attack.
- Tab: inventory; Tab or Escape: close inventory / go back.
- H: field guide with controls and the symbol legend.
- Enter / < / >: use a staircase. Escape: pause, change character/colour, save and quit.
- Settings → Music: 17 tracks copied from the older project are available. You can also add MP3, FLAC or WAV files to `music/`.

The supported terminal minimum is 160 columns × 50 rows. Maximize Terminal or reduce the font size if it asks for more space. Use a monospace font such as Menlo for aligned line art. The map capacity is 300 × 100 cells.

Manual build: `./build-macos.sh` or `make`. Native objects live in `build-macos/obj`, separate from Linux objects. This Mac already has SDL2, SDL2_mixer and ncurses installed under `/opt/homebrew`; no new dependencies were installed.

## Saves and backup

Full saves now use version 2 with shield counts, item flags and projectile ammunition. Legacy profile headers remain readable, but old incomplete full saves are refused instead of loaded unsafely. Start a **New Game** for this review. Successful full saves keep the previous file as `.bak` and replace the current file atomically. Automatic saving retains the full world; temporary battle rooms wait until return to the normal floor.

The untouched original version is in `/Users/arad/Documents/Uni/outputs/rouge-ncurses/review-2026-10-27/backup-before/`. The separately repaired older project remains in `../fop2024-project-AIDGH/`.

## Verification

`./tests/run-memory-checks.sh` builds and runs the ASan/UBSan regression suite in a temporary directory, without touching real player saves. Optional arguments: number of runs, terminal rows, terminal columns. Audio is stubbed for these tests.

See `REVIEW-2026-10-27.md` for scope and remaining validation.

## Accounts and local login

Create a hero with only a username and password, or sign in to an existing account. A successful login is remembered across restarts; **L on the main menu logs out**. Game Menu → Menu returns to the main menu. Credentials now use SQLite with salted PBKDF2 password hashes, and persistent login uses a random local token rather than a saved password.

On first launch, compatible accounts in `users.txt` migrate transactionally into `.local-data/accounts.sqlite3`. The old account file is retained privately as `.local-data/legacy-users-<timestamp>.txt`. The original save files in `users/` remain in place; they are separate from the account database. Do not delete `.local-data` if you want to retain accounts. Its files and bundled local music are excluded from Git.

`./tests/run-accounts-checks.sh` exercises migration, authentication, persistence and logout using disposable fixtures. No real credentials are used in tests.

Character selection now includes 12 additional bold silhouettes (48 total), a live map-cell preview and **H** to toggle the high-contrast player marker. Escape cancels. Icons that occupy more than one terminal cell cannot be equipped, so they cannot overwrite the adjacent map tile. New-mode menus use a connected stonework frame; Origin retains the original decorations.

## Doors, ranged combat, shuffle music and the Mac launcher — 2026-10-28

Start a **New Game** to get the new corridor layout, mandatory password-door placement and larger final arena. Existing saved maps retain their geometry. Extra entrances are removed only when the remaining paths preserve connectivity. Locked doors are placed on essential passages, with code panels reachable on the near side; diagonal moves and sword lunges cannot cut through walls or closed locks. Ancient Keys still unlock doors as designed.

**Ranged weapons:** first press **Space** to aim, then **arrows / WASD** to shoot. Q/Z/C/Page Up also aim diagonally. **Escape** cancels without spending ammo; **E** repeats the last direction; **left click** fires toward the mouse. For Magic Wand, arrows/mouse shoot while standing still (1 charge); WASD rides with the spell (2 charges). Letter diagonal aim also rides. E repeats the last mode and direction. A ride requires 2 available charges and stops before enemies or walls. Missed Wand shots remain recoverable, like the original game. Blocked first steps spend no ammo. The HUD and first-weapon guide explain the two-step controls.

Sword drops increased from 1/6 to 2/8 weapon rolls (about 17% → 25%). Music starts with the selected track, then plays a shuffled cycle without repeats; it advances while waiting for input and respects Pause Music. Select a track through Settings → Music.

The final arena scales to the terminal; the Final Battle countdown is above the dragon portrait. Completed runs cannot trigger the boss again. Enter or Escape leaves the ending screen. Character Change includes a large six-line preview for each of the 48 symbols.

**Mac app:** double-click `build-macos/Rouge Dungeon.app`. This is a local macOS launcher with a custom icon; it opens the existing terminal game and shares its accounts/saves. Keep it in `build-macos`; put an alias in Applications or the Dock rather than moving the bundle. It is not a standalone graphical port. Rebuild it with `./build-macos-app.sh`.

## Immortal encounter — 2026-10-28

The final boss keeps 200 HP, pursuit, contact/aura damage, weapon interactions and existing win rewards. A panel above the dragon shows current health, phase, attack warning and recovery. At half health Immortal enters phase II. Dragonfire locks a row/column at the start of its warning; move off it. Ground slam marks a square around its locked origin; move outside. Warnings last two enemy turns in phase I and one in phase II. Only gameplay actions advance them; menus and aiming cancellation do not. Specials deal 14/18 (fire) or 18/22 (slam) damage in phases I/II, with shield overflow reaching health. There are two recovery turns after a special. The classic weapon moves remain intact. Both Origin and New receive the boss panel and warning overlays. Reopen the game to use the updated executable.

## Victory trophies and expedition summary — 2026-10-28

Defeating Immortal still awards the existing 1000 gold and 40000 × difficulty score. The new victory screen offers one additional trophy: Dragon’s Hoard (+750 gold), Immortal’s Sigil (+12000 × difficulty score), or Traveller’s Cache (+350 gold and +6000 × difficulty score). Use Left/Right or 1/2/3 to preview, Enter to claim, then Enter/Escape to return to the menu. Escape before claiming skips only the extra trophy. A claimed reward cannot be changed or claimed again.

The summary shows session time (excluding time spent choosing the trophy), discovered rooms, defeated enemies, remaining health/shield, boss-special dodges/hits, earned gold/score and profile score. Profiles save after claiming; R retries a failed save. Origin retains its original victory artwork behind the summary. Finalized score/time are preserved when returning to the main menu.
