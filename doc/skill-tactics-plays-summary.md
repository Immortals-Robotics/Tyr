# Skill, Tactics, and Plays system (current C++ implementation)

## What exists today

The current codebase implements a three-layer decision stack:

1. **Skills**: the smallest behavior unit (`Skill::execute(Robot&)`).
2. **Tactics**: role-level controllers (`Tactic::execute(Robot&)`) that orchestrate one or more skills.
3. **Plays**: match-state orchestrators implemented as `Ai` methods that assign robots to roles and run tactics (or direct skills in a few cases).

There is no standalone `Play` base class in the same style as `Skill` / `Tactic`; plays are methods on `Ai` chosen by referee/game context.

## Layer details

### 1) Skills

`Skill` is a simple interface with an `id()` and `execute()` method.

Representative examples:

- `KickBallSkill` plans a behind-ball approach and then commands kick/chip.
- `InterceptBallSkill` predicts interception point and navigates there.
- `MarkToGoalSkill` computes blocking points on own-goal shooting line.

### 2) Tactics

`Tactic` is also a simple interface with `id()` and `execute()`.

Representative compositions:

- `AttackerTactic` is an internal state machine that transitions between interception, wait, turn-and-shoot, and kick states, then calls the corresponding skills (`InterceptBallSkill`, `WaitForBallSkill`, `TurnAndShootSkill`, `KickBallSkill`, `OneTouchSkill`).
- `MarkTactic` chooses between `MarkToGoalSkill` and `MarkToBallSkill` based on opponent-to-goal/ball geometry.
- `ReceivePassTactic` and `DefTactic` are tactic-level robot controllers using direct navigation logic.

### 3) Plays

Plays are implemented as member functions of `Ai`, selected in `Ai::process()` from referee states. Examples include:

- `newNormalPlay()` → baseline role assignment + `GkTactic` / `DefTactic` + either `normalPlayAtt()` or `normalPlayDef()`.
- Set-play handlers such as `kickoffUsPass`, `kickoffTheirOneWall`, `cornerTheirGlobal`, `penaltyUsShootout`, `placeBall`, and `strategy`.

`strategy()` uses a protobuf-backed playbook (`PlayBook` / `Strategy` / `Role` / `Waypoint`) that can be loaded from disk (`strategy.ims`) or received over UDP. Weighted strategy selection is tracked by `m_target_str`.

## How they are connected

The runtime chain is:

1. `Ai::process()` chooses **play** based on referee context.
2. The chosen play assigns role IDs (`m_gk`, `m_def*`, `m_mid*`, `m_attack`) and calls **tactics**.
3. Tactics execute role logic, often delegating to **skills**.
4. Skills generate concrete robot commands (`navigate`, `face`, `shoot`, `chip`, `halt`).

## Practical notes for the C# rewrite

If your C# rewrite is missing this subsystem, preserving this architecture will keep behavior parity:

- Keep `ISkill` and `ITactic` interfaces equivalent to the current C++ contracts.
- Implement plays as either methods on a central `Ai` class (current style) or as objects if you want explicit play polymorphism.
- Preserve the tactic-level state machines (especially attacker/GK), because most match behavior complexity lives there.
- Keep playbook protobuf schema and weighted strategy selection if you need compatibility with existing strategy tooling.

## FSM recommendation: stateful tactics, stateless skills

Your idea makes sense and is consistent with the current code behavior.

- In this codebase, the most meaningful temporal logic already lives in tactics (e.g. attacker and circle-ball state machines).
- Skills are best treated as short-lived, reusable action primitives (kick, intercept, mark, halt) with little or no memory.
- Plays should remain scenario orchestrators (referee/state aware), while tactics own role-level progression.

Recommended split:

1. **Play FSM (coarse):** referee/game-phase transitions (`KickoffUs`, `Stop`, `NormalPlay`, `PlaceBall`, ...).
2. **Tactic FSM (medium):** per-role progression (`Interception -> WaitForBall -> TurnAndShoot -> Kick`, etc.).
3. **Skill actions (fine):** mostly stateless command generators called by tactic states.

Practical implementation notes for C#:

- Keep tactic instances persistent per assigned role/robot so their FSM state is retained across ticks.
- Add explicit `Enter`/`Tick`/`Exit` state handlers in tactics, with transition guards based on world/referee/robot conditions.
- Reset tactic FSMs on major context switches (play switch, role reassignment, robot change, or timeout).
- Keep skills deterministic/pure with only input parameters + current world snapshot; avoid embedding FSM state in skills.

## Catalog: all current skills, tactics, and plays

Use this as a concrete implementation/porting checklist.

### Skills (`source/soccer/skills`)

- **DefenceWallSkill** (`defence_wall.cpp`): positions a robot as a wall blocker between ball/opponent attack lane and own goal (with kickoff variant).  
- **DribbleToDirectionSkill** (`dribble_to_direction.cpp`): keeps control of the ball while steering dribble motion toward a target direction.  
- **HaltSkill** (`halt.cpp`): decelerates/stops robot safely (halt command when needed).  
- **InterceptBallSkill** (`intercept_ball.cpp`): predicts interception timing/point on ball trajectory and navigates to cut it off.  
- **KickBallSkill** (`kick_ball.cpp`): approaches behind the ball with dynamic offset/alignment and applies kick/chip command.  
- **MarkToBallSkill** (`mark_to_ball.cpp`): marks an opponent on the opp→ball line at configured marking distance.  
- **MarkToGoalSkill** (`mark_to_goal.cpp`): marks an opponent on the opp→own-goal shot lane (or defense-area boundary lane).  
- **OldAttackerSkill** (`old_attacker.cpp`): legacy all-in-one attacker behavior (circling/approach/kick/chip/dribble logic).  
- **OneTouchSkill** (`one_touch.cpp`): computes one-touch receive/shoot geometry and triggers immediate shot/chip when aligned.  
- **TurnAndShootSkill** (`turn_and_shoot.cpp`): reorients around the ball to desired shot angle, then delegates to kick behavior.  
- **WaitForBallSkill** (`wait_for_ball.cpp`): holds a receive point on predicted ball line and faces incoming ball.  

### Tactics (`source/soccer/tactics`)

- **AttackerTactic** (`attacker.cpp`): attacker FSM (Interception / WaitForBall / TurnAndShoot / Kick) that orchestrates attack skills.  
- **CircleBallTactic** (`circle_ball.cpp`): close-control circling FSM around the ball to align and then kick/chip.  
- **DefTactic** (`def.cpp`): defender positioning/interception logic (static block lanes + dive behavior when goaling threat exists).  
- **GkTactic** (`gk.cpp`): goalkeeper logic (shot-line positioning, interception/shirje, emergency clear in penalty area).  
- **MarkTactic** (`mark.cpp`): chooses mark-to-goal vs mark-to-ball skill based on threat geometry.  
- **ReceivePassTactic** (`receive_pass.cpp`): receives/presents for pass at target spot while facing ball.  

### Plays (`source/soccer/plays`)

- **haltAll** (`halt.cpp`): global emergency halt for all robots.  
- **stop** (`stop.cpp`): stop-state formation; role assignment + defensive structure + passive attacker position.  
- **newNormalPlay** (`normal_play.cpp`): main running-state dispatcher; executes GK/defs and branches to attack/defense normal modes.  
- **normalPlayAtt** (`normal_play_att.cpp`): attacking normal-play logic (receiver placement, pass-vs-shot choice, attacker execution).  
- **normalPlayDef** (`normal_play_def.cpp`): defensive normal-play logic (marking/static mids + clearance/shot behavior).  
- **kickoffUsPass** (`kickoff_us_pass.cpp`): our kickoff routine built around controlled pass pattern.  
- **kickoffUsChip** (`kickoff_us_chip.cpp`): our kickoff routine built around chip target pattern.  
- **kickoffTheirOneWall** (`kickoff_their_one_wall.cpp`): their kickoff defense with wall + targeted markers and zonal mids.  
- **throwinChipShoot** (`throwin_chip_shoot.cpp`): our free-kick/throw-in style routine using timed chip-or-shoot setup.  
- **cornerTheirGlobal** (`corner_their_global.cpp`): their corner defense with GK/defs, wall attacker, and mark/static mids.  
- **penaltyUsShootout** (`penalty_us_shootout.cpp`): our penalty routine with staged attacker behavior and support positioning.  
- **penaltyTheirSimple** (`penalty_their_simple.cpp`): their penalty defense with GK-centered response and fixed support lines.  
- **placeBall** (`place_ball.cpp`): our ball-placement FSM (kiss/long-distance/stuck handling, coordinated attack-mid5 roles).  
- **theirPlaceBall** (`their_place_ball.cpp`): their ball-placement response with legal-distance positioning and defensive spacing.  
- **strategy** (`strategy.cpp`): playbook-driven set-play executor (role waypoints, timing/position steps, afterlife behavior).  

### Playbook/data helpers used by plays

- **PlayBook / Strategy / Role / Waypoint** (`play_book.h`): protobuf-backed strategy schema consumed by `strategy()`.  
- **loadPlayBook / receivePlayBook / setPlayBook** (`strategy.cpp`): disk/network ingestion and validation of strategy data.  
