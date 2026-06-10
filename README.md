


# kitty_tamagochi

|.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.|
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|.|.|^|.|.|.|^|.||.|.|^|.|.|.|^|.||.|^|.|.|.|^|.|.||.|^|.|.|.|^|.|.|
|.|/|.|\\ |.|/|.|\\ ||.|/|.|\\ |.|/|.|\\ ||/|.|\\ |.|/|.|\\ |.||/|.|\\ |.|/|.|\\ |.|
|.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.|
|=|=|.|#|.|#|.|=||=|=|.|.|.|.|.|=||=|.|.|.|.|.|=|=||=|.|#|.|#|.|=|=|
|.|.|.|.|\* |.|.|.||.|.|.|#|.|#|.|.||.|.|#|.|#|.|.|.||.|.|.|\* |.|.|.|.|
|=|=|.|.|.|.|.|=||=|=|.|.|\*|.|.|=||=|.|.|\* |.|.|=|=||=|.|.|.|.|.|=|=|
|.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.||.|.|.|.|.|.|.|.|


> Interactive virtual pet inspired by 90s digital pets, powered by an **Elegoo UNO R3** and a **MAX7219 LED Dot Matrix**.


## Technical Stack

* **Hardware:** Elegoo UNO R3, 8x8 MAX7219 LED Matrix, 5x Pushbutton
* **Libraries:** `LedControl`, `ezButton`
* **Language:** Embedded C++



### Key Features

* **Dynamic State Machine:** Handles menu navigation, game states, and sleep cycles without blocking user input.
* **Time-Based Decay:** Stat systems (Hunger, Energy, Happiness, Health) operate on an independent background clock to simulate real-time care.
* **Interactive Mini-Games:** Includes a "Purr" mashing game and a "Laser Pointer" reaction game.
* **Non-Blocking Logic:** Uses `millis()` for animation timing to ensure the device remains responsive to button inputs at all times.
* **Animatios:** Sprite system allowing the pet to face different directions and interact with items.



## Demonstration

>The faces of the pet


https://github.com/user-attachments/assets/0c7b694c-c602-482c-8ba7-945563edcc5c

>The happines, hunger, energy and health menus


https://github.com/user-attachments/assets/93b86942-9192-4274-8c56-f44290d25eea




## Program Logic

### State Management

The project uses an `enum` based state machine to ensure only one logic block runs at a time, preventing input conflicts:

```cpp
enum AppState { CAT_MENU, MAIN_MENU, SELECTING_GAME, SELECTING_FOOD, SLEEPING };

```

### Time-Based Decay

Stats are managed by independent counters to ensure that long-term decay doesn't interfere with short-term needs.

| Stat | Decay Rate | Goal |
| --- | --- | --- |
| **Hunger** | 1pt / Hour | 0 in 8 Hours |
| **Energy** | 1pt / 1.5 Hours | 0 in 12 Hours |
| **Happiness** | 1pt / 3 Hours | 0 in 24 Hours |
|**Health**|4pt/2~5 Days | 0 in 4~10 Days|

### Mini-Game Architecture

The **Laser Pointer Game** uses coordinate-based logic. The screen is split into four 4x4 zones, mapping directly to physical directional buttons, requiring rapid reaction from the user.



##  Project Structure

```cpp
Cat_menu
|
| - Default
| - Look right down
| - Look left up
| - Look left down

Main_menu (replenishes stat levels)
|
| - Hunger 
|   |
|   | - Fish +2
|   | - Chicken +4
|   | - Bug +1     //also gives +1 happiness
|   
| - Happiness 
|   |
|   | - Laser Game +2 if caught lasers > 10 in 15 second
|   | - Purr mash +1 if buttons mashed > 20 in 10 seconds
|
| - Energy
|   |
|   |-Sleep +8 if sleep > 4 hours
|
| - Health
|   |
|   | - Vaccine +4
```
---

>Robotics course
>June 2026

---

## The face behind the cat sprite
\
<img width="200" alt="KITTY_PIC" src="https://github.com/user-attachments/assets/73a10287-aff7-45a2-bbcf-1b8ae6516a21" />
> My cat from home














