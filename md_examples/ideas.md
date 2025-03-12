* Check if start with #
    peek ahead until no more # found
    decide heading from that

* Check if start with *
    ```check boolean if already in unordered list
        if already in add ul element
        if not create ul 
--      PARSE ENTIRE LINE
-    ```check if next line starts with *
        if not end ul list tag

* check if start with ($NUM).
    ```check boolean if already in ordered list
        if already add ol element
        if not creat ol
--      PARSE ENTIRE LINE
-   ```check if next line starts with ($NUM) order
        if not end ol list tag

* check if start with >
    ```check if already in blockquote
        if already in add nothing?
        if not create blockquote
--      PARSE ENTIRE LINE
-   ```check if next line starts wth >
        if not end blockquote tag

    
    
