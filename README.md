# (DEPRECATED) pg-epics
A set of python2 scripts that read/writes EPICS Process Variables (PVs) using a postgres-based message queue to make requests.

### LS-CAT Notes
- pg-epics is used only to control the monochromator.
- All legacy software on LS-CAT's cluster uses pg_advisory_lock() to manage access to beamline controls. This includes:
  1. Reading/writing EPICS PVs controlling the Kohzu monochromator on station D.
  2. Issuing commands to Dectris and legacy Rayonix (MarCCD) detectors.
  3. Controlling the legacy MD2 microdiffractometer.
  4. Issuing commands which move the legacy CATS-based sample-changing robots.
- Future software development shall disregard all postgres-based locks.
