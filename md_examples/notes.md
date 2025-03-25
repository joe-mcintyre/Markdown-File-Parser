# Black Box Testing
1. Concepts
I. What relationship is there between equivalence partitions of a given program
* They are grouped together by similar inputs
* so theres no overlap & they collectively cover the whole domain

II. What inputs should be chosen when performing Equivalence Partitioning
* Choose 1 value type for each partition


III. What inputs should be chosen when performing equivalence partitioning & boundary value analysis
* 1 jus below the boundary & 1 just above

2. Password Length
I. In a Web-based application, a password field should accept minimum 6 characters
and maximum 10 characters. Apply the Equivalence Partitioning and Boundary Value Analysis techniques to test the password checking program
* Less then 6 characters & greater then 10 characters

3. Password Tester :Based on the documentation for the PasswordTester class (see Appendix), answer the following questions
I. How many equivalence partitions would there be for this program
    1. length:              >= 8 or < 8
    2. lowercase_letter     YES or NO
    3. uppercase_letter     YES or NO
    4. Number               YES or NO
    5. Symbol               YES or NO

    Fail No Critera (Parition 1)
    Fail 1 Critera (Parition 2-6)
    Fail 2 Critera (Parition 7-16): C^2↓5 = (5 * 4) / (2 * 1) = 10
    Fail 3 Critera (Parition 17-26): C^2↓5 = (5 * 4) / (2 * 1) = 10
    Fail 4 (Parition 27-31)
    Fail All (Parition 32)

II. What inputs would be required to test these partitions
    * aB#1 (not long enough)
    * PSSWRD1#! (no lower case)
    * ads#!11p (no capital letter)
    * PwOrd!@# (no number)
    * Password1 (no symbol)


4. Online Payment System Testing

5. Blackbox Testing of BuggyCalculator Application
